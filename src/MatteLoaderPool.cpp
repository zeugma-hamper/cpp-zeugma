
//
// (c) treadle & loam, provisioners llc
//

#include <MatteLoaderPool.hpp>

#include <Matte.hpp>

#include <OpenImageIO/imageio.h>

#include <gstreamer-1.0/gst/video/video-info.h>

#include <algorithm>

#include <BlockTimer.hpp>

namespace fs = std::filesystem;


namespace zeugma  {


MatteLoaderWorker::MatteLoaderWorker (f32 _start_time, u32 _frame_count,
                                      fs::path const &_matte_dir)
  : m_paths {},
    m_starting_pts {_start_time},
    m_starting_frame_id {-1},
    m_loader_pool {nullptr}
{
  m_load_jobs.set_capacity (2);
  m_result_queue.set_capacity(4);

  m_paths.reserve (_frame_count);
  {
    auto const end = fs::directory_iterator {};
    auto cur = fs::directory_iterator{_matte_dir};
    for (; cur != end; ++cur)
      m_paths.push_back(*cur);
  }

  std::sort (m_paths.begin (), m_paths.end ());
}

MatteLoaderWorker::~MatteLoaderWorker ()
{
  m_loader_pool = nullptr;

  {
    std::unique_lock lock {m_job_mutex};
    m_load_jobs.clear ();
  }

  {
    std::unique_lock lock {m_result_mutex};
    m_result_queue.clear();
  }
}

void MatteLoaderWorker::SetMatteLoaderPool (MatteLoaderPool *_pool)
{
  m_loader_pool = _pool;
}


void MatteLoaderWorker::PushJob (u32 _frame, i32 fps_num, i32 fps_denom)
{
  {
    std::unique_lock lock {m_result_mutex};
    // already loaded so bail
    for (szt i = 0; i < m_result_queue.size (); ++i)
      if (m_result_queue[i].offset == _frame)
        return;
  }

  // fprintf (stderr, "mlw push job: %u\n", _frame);
  std::unique_lock lock {m_job_mutex};
  if (m_starting_frame_id < 0)
    {
      m_starting_frame_id
        = (i64)std::round (m_starting_pts * fps_num / fps_denom);
      fprintf (stderr, "starting frame id is %ld from %.3f\n", m_starting_frame_id, m_starting_pts);
    }

  //if out of bounds
  if (i64 (_frame) < m_starting_frame_id ||
      _frame >= m_paths.size () + m_starting_frame_id)
    return;

  //if already scheduled to load
  i64 const fnum = _frame;
  for (LoadJob const &lj : m_load_jobs)
    if (lj.frame_id == fnum)
      return;

  //pop oldest job if full
  if (m_load_jobs.size () == m_load_jobs.capacity())
    m_load_jobs.pop_front();

  m_load_jobs.push_back({fnum});
  if (m_load_jobs.size () == 1)
    m_loader_pool->SetReady(ch_from_this ());
}

void MatteLoaderWorker::DoWork ()
{
  i64 frame_id = -1;
  {
    std::unique_lock lock {m_job_mutex};
    if (m_load_jobs.size () > 0)
      {
        frame_id = m_load_jobs.front ().frame_id;
        m_load_jobs.pop_front();

        if (m_load_jobs.size () > 0)
          m_loader_pool->SetReady(ch_from_this ());
      }
  }

  if (frame_id < 0)
    return;

  bgfx::TextureFormat::Enum const formats[5]{
    bgfx::TextureFormat::Unknown,
    bgfx::TextureFormat::R8,
    bgfx::TextureFormat::RG8,
    bgfx::TextureFormat::RGB8,
    bgfx::TextureFormat::RGBA8};

  u32 const index = u32 ((frame_id - m_starting_frame_id) % m_paths.size ());
  auto &path = m_paths[index];

  std::unique_ptr<OIIO::ImageInput> iinput =
    OIIO::ImageInput::open(path.string());

  if (!iinput)
    {
      fprintf(stderr, "error loading %s\n", path.c_str());
      return;
    }

  OIIO::ImageSpec ispec = iinput->spec();
  i32 const max_dim = 1 << 15;
  i32 const max_channels = 4;
  assert(ispec.width <= max_dim && ispec.height <= max_dim &&
         ispec.nchannels < max_channels);

  u32 size = ispec.width * ispec.height * ispec.nchannels;
  u8_ptr data;

  {
    data = u8_ptr ((u8 *)malloc(size));
    //BlockTimer timer ("load image from disk");
    iinput->read_image(0, 0, 0, 1, OIIO::TypeDesc::UINT8, data.get ());
    iinput->close();
  }
  {
    std::unique_lock lock {m_result_mutex};
    m_result_queue.push_back (MatteFrameUnique {u32 (frame_id),
                                                formats[ispec.nchannels],
                                                u32(ispec.width),
                                                u32(ispec.height),
                                                size,
                                                std::move (data)});
  }

  // bimg_ptr img;
  // {
  //   BlockTimer load ("load frame from disk");
  //   u32 const index = u32 ((frame_id - m_starting_frame_id) % m_paths.size ());
  //   auto &path = m_paths[index];
  //   bx::Error error;
  //   img = LoadKTXImage(path.c_str(), &error);
  //   if (! img)
  //     return;
  // }

  // {
  //   std::unique_lock lock {m_result_mutex};
  //   m_result_queue.push_back (MatteFrameBimg {u32 (frame_id), std::move (img)});
  // }
}

bool
MatteLoaderWorker::PopFrame (u32 _id, MatteFrameUnique &_frame)
{
  std::unique_lock lock {m_result_mutex};

  auto const end = m_result_queue.end ();
  for (auto cur = m_result_queue.begin (); cur != end; ++cur)
    if (cur->offset == _id)
      {
        _frame = std::exchange (*cur, {});
        return true;
      }

  return false;
}

bool
MatteLoaderWorker::PopAndReleaseFrames (u32 _id, MatteFrameUnique &_frame)
{
  std::unique_lock lock {m_result_mutex};

  auto const end = m_result_queue.end ();
  for (auto cur = m_result_queue.begin (); cur != end; ++cur)
    if (cur->offset == _id)
      {
        _frame = std::exchange (*cur, {});
        m_result_queue.erase(m_result_queue.begin (), cur+1);
        return true;
      }

  m_result_queue.clear ();
  return false;
}

static u64 const s_ml_pool_thread_count = 12u;

MatteLoaderPool::MatteLoaderPool ()
  : m_currently_loading {true}
{
  m_loading_threads.reserve(s_ml_pool_thread_count);
  for (u64 i = 0; i < s_ml_pool_thread_count; ++i)
    m_loading_threads.emplace_back (MatteLoaderPool::LoadMattes, this);
}

MatteLoaderPool::~MatteLoaderPool ()
{
  ShutDown ();
}

void MatteLoaderPool::ShutDown()
{
  m_currently_loading.store (false);
  m_worker_cond_var.notify_all();
  for (std::thread &t : m_loading_threads)
    t.join ();

  m_loading_threads.clear ();
  m_workers.clear();
}

void MatteLoaderPool::SetReady (ch_ptr<MatteLoaderWorker> const &_worker)
{
  if (! m_currently_loading.load ())
    return;

  std::unique_lock lock {m_worker_mutex};
  m_workers.push_back (ch_weak_ptr<MatteLoaderWorker>{_worker});
  m_worker_cond_var.notify_one();
}

ch_ptr<MatteLoaderWorker> MatteLoaderPool::NextWorker ()
{
  std::unique_lock lock {m_worker_mutex};
  while (m_currently_loading.load())
    {
      if (m_workers.size() > 0)
        {
          auto nxt = m_workers.front().lock();
          m_workers.erase(m_workers.begin ());
          return nxt;
        }

      m_worker_cond_var.wait (lock);
    }

  return {};
}

void MatteLoaderPool::LoadMattes (MatteLoaderPool *pool)
{
  while (pool->m_currently_loading.load ())
    {
      if (ch_ptr<MatteLoaderWorker> worker = pool->NextWorker (); worker)
        worker->DoWork ();
    }
}


}
