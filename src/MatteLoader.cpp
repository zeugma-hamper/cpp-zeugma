#include <MatteLoader.hpp>

#include <algorithm>
#include <chrono>

#include <OpenImageIO/imageio.h>

namespace charm
{

static u32 max_loaded_frames = 3;

namespace fs = std::filesystem;

MatteLoader::MatteLoader ()
  : m_matte_dir {},
    m_keep_running {false},
    m_dir_frame_count {0},
    m_requested_offset {0}
{
}

MatteLoader::~MatteLoader ()
{
  Shutdown ();

  for (MatteFrame &frame : m_frames)
    free (frame.data);

  m_frames.clear ();
}

void MatteLoader::StartLoading (std::filesystem::path const &_dir)
{
  m_matte_dir = _dir;

  m_dir_frame_count = std::distance (fs::directory_iterator {m_matte_dir},
                                     fs::directory_iterator {});

  m_matte_paths.insert(m_matte_paths.end (),
                       fs::directory_iterator {m_matte_dir},
                       fs::directory_iterator {});
  std::sort (m_matte_paths.begin (), m_matte_paths.end ());

  m_requested_offset = 0u;
  m_keep_running.store (true);
  m_thread = std::thread ([this] () {MatteLoader::LoadFrames(this); });
}

void MatteLoader::Shutdown ()
{
  m_keep_running.store (false);
  m_frame_cond.notify_all();
  m_thread.join();
}

static u32 ring_distance (u32 _cur, u32 _last, u32 _max)
{
  if (_cur <= _last)
    return _last - _cur;

  return (_max - _cur) + _last;
}

static u32 ring_next (u32 _cur, u32 _last, u32 _max)
{
  u32 l = _last % _max;
  if (ring_distance (_cur, l, _max) == 0)
    return u32 (-1);

  return (_cur + 1) % _max;
}

u32 MatteLoader::FindNextOffset ()
{
  std::unique_lock lock {m_frame_mutex};
  while (true)
    {
      //if we've loaded no mattes, load the last requested matte
      if (m_frames.empty())
        return (m_requested_offset) % m_dir_frame_count;
      else //otherwise load the matte after the previously loaded matte
        {
          u32 next = ring_next (m_frames.back ().offset, m_requested_offset + max_loaded_frames,
                                m_dir_frame_count);
          if (next != u32 (-1))
            return next;
        }

      //wait if we've loaded enough buffers
      m_frame_cond.wait(lock);
    }

  return u32(-1);
}

void MatteLoader::StoreMatte (MatteFrame const &_mf)
{
  std::unique_lock lock {m_frame_mutex};
  m_frames.push_back (_mf);
}

MatteFrame MatteLoader::GetFrame (u32 _offset)
{
  std::unique_lock lock {m_frame_mutex};
  auto end = m_frames.end ();
  for (auto cur = m_frames.begin(); cur != end; ++cur)
    {
      if (cur->offset == _offset)
        {
          MatteFrame frame = std::exchange (*cur, MatteFrame {});
          std::for_each (m_frames.begin (), cur+1,
                         [] (MatteFrame &f) { free (f.data); });
          m_frames.erase (m_frames.begin (), cur+1);
          m_requested_offset = _offset;
          m_frame_cond.notify_one ();
          return frame;
        }
    }

  // didn't find what we're looking for so trash everything
  m_requested_offset = _offset;
  std::for_each (m_frames.begin (), m_frames.end (),
                 [] (MatteFrame &f) { free (f.data); });
  m_frames.clear ();
  m_frame_cond.notify_one();
  return {};
}

void MatteLoader::LoadFrames (MatteLoader *_loader)
{
  while (_loader->m_keep_running.load ())
    {
      //determine next matte to load
      //u32 max signifies no loading is required right now
      u32 next = _loader->FindNextOffset();

      //ok, actually load from disk
      fs::directory_entry matte_on_deck = _loader->m_matte_paths[next];
      if (matte_on_deck.path().empty())
        {
          fprintf (stderr, "empty path on loading %s\n", _loader->m_matte_dir.c_str());
          continue;
        }

      bgfx::TextureFormat::Enum const formats[5]
        { bgfx::TextureFormat::Unknown,
          bgfx::TextureFormat::R8,
          bgfx::TextureFormat::RG8,
          bgfx::TextureFormat::RGB8,
          bgfx::TextureFormat::RGBA8
        };

      std::unique_ptr<OIIO::ImageInput> iinput
        = OIIO::ImageInput::open (matte_on_deck.path ().string ());

      if (! iinput)
        {
          fprintf (stderr, "error loading %s\n", matte_on_deck.path ().c_str());
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

      MatteFrame frame {next, formats[ispec.nchannels],
                        u32(ispec.width), u32(ispec.height), size, data};
      _loader->StoreMatte(frame);
    }
}

void MatteLoader::MatteDataReleaseFn (void *_ptr, void *)
{
  free (_ptr);
}

MatteLoaderBimg::MatteLoaderBimg ()
  : m_matte_dir {},
    m_keep_running {false},
    m_dir_frame_count {0},
    m_requested_offset {0}
{
}

MatteLoaderBimg::~MatteLoaderBimg ()
{
  Shutdown ();

  m_frames.clear ();
}

void MatteLoaderBimg::StartLoading (std::filesystem::path const &_dir)
{
  m_matte_dir = _dir;

  m_dir_frame_count = std::distance (fs::directory_iterator {m_matte_dir},
                                     fs::directory_iterator {});

  m_matte_paths.insert(m_matte_paths.end (),
                       fs::directory_iterator {m_matte_dir},
                       fs::directory_iterator {});
  std::sort (m_matte_paths.begin (), m_matte_paths.end ());

  m_requested_offset = 0u;
  m_keep_running.store (true);
  m_thread = std::thread ([this] () {MatteLoaderBimg::LoadFrames(this); });
}

void MatteLoaderBimg::Shutdown ()
{
  m_keep_running.store (false);
  m_frame_cond.notify_all();
  m_thread.join();
}

u32 MatteLoaderBimg::FindNextOffset ()
{
  std::unique_lock lock {m_frame_mutex};
  while (true)
    {
      //if we've loaded no mattes, load the last requested matte
      if (m_frames.empty())
        return (m_requested_offset) % m_dir_frame_count;
      else //otherwise load the matte after the previously loaded matte
        {
          u32 next = ring_next (m_frames.back ().offset, m_requested_offset + max_loaded_frames,
                                m_dir_frame_count);
          if (next != u32 (-1))
            return next;
        }

      //wait if we've loaded enough buffers
      m_frame_cond.wait(lock);
    }

  return u32(-1);
}

void MatteLoaderBimg::StoreMatte (MatteFrameBimg &&_mf)
{
  std::unique_lock lock {m_frame_mutex};
  m_frames.push_back (std::move (_mf));
}

MatteFrameBimg MatteLoaderBimg::GetFrame (u32 _offset)
{
  std::unique_lock lock {m_frame_mutex};
  auto end = m_frames.end ();
  for (auto cur = m_frames.begin(); cur != end; ++cur)
    {
      if (cur->offset == _offset)
        {
          MatteFrameBimg frame = std::exchange (*cur, MatteFrameBimg {});
          m_frames.erase (m_frames.begin (), cur+1);
          m_requested_offset = _offset;
          m_frame_cond.notify_one ();
          return frame;
        }
    }

  // didn't find what we're looking for so trash everything
  m_requested_offset = _offset;
  m_frames.clear ();
  m_frame_cond.notify_one();
  return {};
}

void MatteLoaderBimg::LoadFrames (MatteLoaderBimg *_loader)
{
  while (_loader->m_keep_running.load ())
    {
      //determine next matte to load
      //u32 max signifies no loading is required right now
      u32 next = _loader->FindNextOffset();

      //ok, actually load from disk
      fs::directory_entry matte_on_deck = _loader->m_matte_paths[next];
      if (matte_on_deck.path().empty())
        {
          fprintf (stderr, "empty path on loading %s\n", _loader->m_matte_dir.c_str());
          continue;
        }

      bx::Error error;
      bimg_ptr img = LoadKTXImage(matte_on_deck.path().c_str(), &error);

      _loader->StoreMatte(MatteFrameBimg {next, std::move (img)});
    }
}


MatteLoaderDispatcher::MatteLoaderDispatcher ()
  : m_loader_pool {nullptr}
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


void MatteLoaderDispatcher::PushJob (u32 _frame, std::filesystem::path const &_path)
{
  std::unique_lock lock {m_job_mutex};
  for (LoadJob const &lj : m_job_queue)
    if (lj.frame_id == _frame)
      return;

  LoadJob job{_frame, _path, {}};
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
  m_loading_threads.reserve(12);
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
      u64 const count = m_dispatchers.size ();
      u64 const start_index = std::min (m_dispatcher_current_index, count);

      u64 &mdci = m_dispatcher_current_index;
      for (; mdci < count; ++mdci)
        if (auto dp = m_dispatchers[mdci].lock (); dp && dp->PopJob(_job))
          {
            ++mdci;
            remove_nulls();
            return true;
          }

      for (mdci = 0u; mdci < start_index; ++mdci)
        if (auto dp = m_dispatchers[mdci].lock (); dp && dp->PopJob(_job))
          {
            ++mdci;
            remove_nulls();
            return true;
          }


      remove_nulls();
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
