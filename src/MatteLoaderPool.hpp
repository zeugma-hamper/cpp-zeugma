#ifndef MATTE_LOADER_POOL_HAS_A_SHALLOW_END
#define MATTE_LOADER_POOL_HAS_A_SHALLOW_END

#include <base_types.hpp>
#include <bgfx_utils.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <boost/circular_buffer.hpp>

#include <filesystem>
#include <future>
#include <mutex>
#include <thread>

namespace charm
{

template<typename T>
struct default_free
{
  void operator () (T *_t) const
  {
    free (_t);
  }
};

using u8_ptr = std::unique_ptr<u8, default_free<u8>>;

struct MatteFrameUnique
{
  u32 offset = u32 (-1);
  bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Unknown;
  u32 width = 0;
  u32 height = 0;
  u32 data_size = 0; //what will i do for images greater than 4GB?
  u8_ptr data;

  bool IsValid () const
  {
    return (bool)data;
  }
};

struct MatteFrameBimg
{
  u32 offset = 0;
  bimg_ptr data;
};

class MatteLoaderPool;
struct ClipInfo;

class MatteLoaderWorker : public CharmBase<MatteLoaderWorker, MTReferenceCounter>
{
 public:
  struct LoadJob
  {
    i64 frame_id;
  };

  MatteLoaderWorker (f32 _start_time, u32 _frame_count,
                     std::filesystem::path const &_matte_dir);

  CHARM_DELETE_MOVE_COPY(MatteLoaderWorker);

  ~MatteLoaderWorker ();

  void SetMatteLoaderPool (MatteLoaderPool *_pool);

  void PushJob (u32 _frame, i32 fps_num, i32 fps_denom);
  void DoWork ();

  bool PopFrame (u32 _id, MatteFrameUnique &_frame);
  bool PopAndReleaseFrames (u32 _id, MatteFrameUnique &_frame);

 protected:
  std::vector<std::filesystem::path> m_paths;
  f32 m_starting_pts;
  i64 m_starting_frame_id;

  std::mutex m_job_mutex;
  boost::circular_buffer<LoadJob> m_load_jobs;

  std::mutex m_result_mutex;
  boost::circular_buffer<MatteFrameUnique> m_result_queue;

  MatteLoaderPool *m_loader_pool;
};

class MatteLoaderPool
{
 public:
  MatteLoaderPool ();
  ~MatteLoaderPool ();

  CHARM_DELETE_MOVE_COPY(MatteLoaderPool);

  void ShutDown ();

  void SetReady (ch_ptr<MatteLoaderWorker> const &_worker);

  ch_ptr<MatteLoaderWorker> NextWorker ();

 protected:
  static void LoadMattes (MatteLoaderPool *pool);

  std::atomic_bool m_currently_loading;
  std::vector<std::thread> m_loading_threads;

  std::mutex m_worker_mutex;
  std::condition_variable m_worker_cond_var;
  std::vector<ch_weak_ptr<MatteLoaderWorker>> m_workers;
};

}

#endif
