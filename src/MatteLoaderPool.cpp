#include <MatteLoaderPool.hpp>

#include <algorithm>

#include <OpenImageIO/imageio.h>

namespace charm
{

MatteLoaderDispatcher::MatteLoaderDispatcher (std::vector<std::filesystem::path> &&_matte_paths)
  : m_paths {std::move (_matte_paths)},
    m_loader_pool {nullptr}
{
  m_job_queue.set_capacity(4);
  m_result_queue.set_capacity(4);
}

MatteLoaderDispatcher::~MatteLoaderDispatcher ()
{
  m_loader_pool = nullptr;

  {
    std::unique_lock lock {m_job_mutex};
    m_job_queue.clear();
  }

  {
    std::unique_lock lock {m_result_mutex};
    m_result_queue.clear();
  }
}

void MatteLoaderDispatcher::SetMatteLoaderPool (MatteLoaderPool *_pool)
{
  m_loader_pool = _pool;
}


void MatteLoaderDispatcher::PushJob (u32 _frame)
{
  u32 const fnum = _frame % m_paths.size ();
  std::unique_lock lock {m_job_mutex};
  for (LoadJob const &lj : m_job_queue)
    if (lj.frame_id == fnum)
      return;

  LoadJob job{_frame, m_paths[fnum], {}};
  LoadResult result{_frame, job.promise.get_future()};
  m_job_queue.push_back (std::move (job));
  lock.unlock();

  {
    std::unique_lock res_lock {m_result_mutex};
    m_result_queue.push_back(std::move (result));
  }
}

bool MatteLoaderDispatcher::HasJobs ()
{
  std::unique_lock lock {m_job_mutex};
  return !m_job_queue.empty();
}

bool MatteLoaderDispatcher::PopJob (LoadJob &_job)
{
  std::unique_lock lock {m_job_mutex};
  if (m_job_queue.empty())
    return false;

  _job = std::move (m_job_queue.front ());
  m_job_queue.pop_front();
  return true;
}

MatteLoaderDispatcher::LoadStatus
MatteLoaderDispatcher::PopFrame (u32 _id, MatteFrameUnique &_frame)
{
  std::unique_lock lock {m_result_mutex};

  auto const end = m_result_queue.end ();
  for (auto cur = m_result_queue.begin (); cur != end; ++cur)
    if (cur->frame_id == _id)
      {
        if (! cur->frame.valid())
          return LoadStatus::Loading;

        _frame = cur->frame.get ();
        m_result_queue.erase(cur);
        return LoadStatus::Loaded;
      }

  return LoadStatus::NotLoading;
}

MatteLoaderDispatcher::LoadStatus
MatteLoaderDispatcher::PopAndReleaseFrames (u32 _id, MatteFrameUnique &_frame)
{
  std::unique_lock lock {m_result_mutex};

  auto const end = m_result_queue.end ();
  for (auto cur = m_result_queue.begin (); cur != end; ++cur)
    if (cur->frame_id == _id)
      {
        if (! cur->frame.valid())
          {
            m_result_queue.erase(m_result_queue.begin (), cur);
            return LoadStatus::Loading;
          }

        _frame = cur->frame.get ();
        m_result_queue.erase(m_result_queue.begin (), cur+1);
        return LoadStatus::Loaded;
      }

  return LoadStatus::NotLoading;
}

static u64 const s_ml_pool_thread_count = 12u;

MatteLoaderPool::MatteLoaderPool ()
  : m_currently_loading {true},
    m_dispatcher_current_index {0u}
{
  m_loading_threads.reserve(s_ml_pool_thread_count);
  for (u64 i = 0; i < s_ml_pool_thread_count; ++i)
    m_loading_threads.emplace_back (MatteLoaderPool::LoadMattes, this);
}

MatteLoaderPool::~MatteLoaderPool ()
{
  assert (! m_currently_loading.load ());
}

void MatteLoaderPool::ShutDown()
{
  m_currently_loading.store (false);
  m_dispatcher_cond_var.notify_all();
  for (std::thread &t : m_loading_threads)
    t.join ();

  m_loading_threads.clear ();
  m_dispatchers.clear();
}

void MatteLoaderPool::AddDispatcher (ch_weak_ptr<MatteLoaderDispatcher> const &_dispatcher)
{
  {
    std::unique_lock lock {m_dispatcher_mutex};
    m_dispatchers.push_back (_dispatcher);
  }

  m_dispatcher_cond_var.notify_one();
}

void MatteLoaderPool::AddDispatcher (ch_weak_ptr<MatteLoaderDispatcher> &&_dispatcher)
{
  {
    std::unique_lock lock {m_dispatcher_mutex};
    m_dispatchers.push_back (std::move (_dispatcher));
  }

  m_dispatcher_cond_var.notify_one();
}

void MatteLoaderPool::NotifyOfJob()
{
  if (m_currently_loading.load())
    m_dispatcher_cond_var.notify_one();
}

bool MatteLoaderPool::NextJob (MatteLoaderDispatcher::LoadJob &_job)
{
  // bog simple round robin at first

  std::unique_lock lock {m_dispatcher_mutex};

  auto remove_nulls = [&dp = m_dispatchers] ()
  {
    auto null_pred = [] (ch_weak_ptr<MatteLoaderDispatcher> const &_ptr) { return !_ptr; };
    auto poofed = std::remove_if (dp.begin (), dp.end (), null_pred);
    dp.erase(poofed, dp.end ());
  };

  while (m_currently_loading.load())
    {
      u64 &mdci = m_dispatcher_current_index;
      u64 const count = m_dispatchers.size ();
      u64 const start_index = mdci;

      for (; mdci < count; ++mdci)
        if (auto dp = m_dispatchers[mdci].lock (); dp && dp->PopJob(_job))
          {
            ++mdci;
            return true;
          }

      //made it through list, do some clean up
      remove_nulls();
      u64 const tidied_start_index = std::min (start_index, m_dispatchers.size ());

      for (mdci = 0u; mdci < tidied_start_index; ++mdci)
        if (auto dp = m_dispatchers[mdci].lock (); dp && dp->PopJob(_job))
          {
            ++mdci;
            return true;
          }


      m_dispatcher_cond_var.wait (lock);
    }

  return false;
}

void MatteLoaderPool::LoadMattes (MatteLoaderPool *pool)
{
  MatteLoaderDispatcher::LoadJob job;
  while (pool->NextJob(job))
    {
      bgfx::TextureFormat::Enum const formats[5]
        { bgfx::TextureFormat::Unknown,
          bgfx::TextureFormat::R8,
          bgfx::TextureFormat::RG8,
          bgfx::TextureFormat::RGB8,
          bgfx::TextureFormat::RGBA8
        };

      std::unique_ptr<OIIO::ImageInput> iinput
        = OIIO::ImageInput::open (job.path.string());

      if (! iinput)
        {
          fprintf (stderr, "error loading %s\n", job.path.c_str());
          continue;
        }

      OIIO::ImageSpec ispec = iinput->spec ();
      i32 const max_dim = 1 << 15;
      i32 const max_channels = 4;
      assert (ispec.width <= max_dim && ispec.height <= max_dim && ispec.nchannels < max_channels);

      u32 size = ispec.width * ispec.height * ispec.nchannels;
      u8 *data = (u8 *)malloc (size);

      iinput->read_image (OIIO::TypeDesc::UINT8, data);
      iinput->close ();

      MatteFrameUnique frame {job.frame_id, formats[ispec.nchannels],
        u32(ispec.width), u32(ispec.height), size, u8_ptr{data}};
      job.promise.set_value (std::move (frame));
    }
}

}
