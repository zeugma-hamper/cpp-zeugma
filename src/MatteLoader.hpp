#ifndef MATTE_LOADER
#define MATTE_LOADER

#include <base_types.hpp>
#include <bgfx_utils.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <boost/circular_buffer.hpp>

#include <future>
#include <thread>
#include <mutex>
#include <vector>
#include <filesystem>
#include <condition_variable>
#include <atomic>

namespace charm
{

struct MatteFrame
{
  u32 offset = 0;
  bgfx::TextureFormat::Enum format;
  u32 width = 0;
  u32 height = 0;
  u32 data_size = 0; //what will i do for images greater than 4GB?
  u8 *data = nullptr;
};

struct MatteFrameBimg
{
  u32 offset = 0;
  bimg_ptr data;
};

class MatteLoader
{
 public:
  MatteLoader ();
  ~MatteLoader ();

  CHARM_DELETE_MOVE_COPY(MatteLoader);

  void StartLoading (std::filesystem::path const &_dir);

  MatteFrame GetFrame (u32 _offset);

  static void LoadFrames (MatteLoader *_loader);
  //for use with bgfx::makeRef
  static void MatteDataReleaseFn (void *_ptr, void *_user_data);

 protected:
  void Shutdown ();

  u32 FindNextOffset ();
  void StoreMatte (MatteFrame const &_mf);


  std::filesystem::path m_matte_dir;
  std::vector<std::filesystem::directory_entry> m_matte_paths;
  std::thread m_thread;
  std::mutex m_frame_mutex;
  std::condition_variable m_frame_cond;
  std::atomic<bool> m_keep_running;
  u32 m_dir_frame_count;
  u32 m_requested_offset;
  std::vector<MatteFrame> m_frames;
};

class MatteLoaderBimg
{
 public:
  MatteLoaderBimg ();
  ~MatteLoaderBimg ();

  CHARM_DELETE_MOVE_COPY(MatteLoaderBimg);

  void StartLoading (std::filesystem::path const &_dir);

  MatteFrameBimg GetFrame (u32 _offset);

  static void LoadFrames (MatteLoaderBimg *_loader);

 protected:
  void Shutdown ();

  u32 FindNextOffset ();
  void StoreMatte (MatteFrameBimg &&_mf);


  std::filesystem::path m_matte_dir;
  std::vector<std::filesystem::directory_entry> m_matte_paths;
  std::thread m_thread;
  std::mutex m_frame_mutex;
  std::condition_variable m_frame_cond;
  std::atomic<bool> m_keep_running;
  u32 m_dir_frame_count;
  u32 m_requested_offset;
  std::vector<MatteFrameBimg> m_frames;
};

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
  u32 offset = 0;
  bgfx::TextureFormat::Enum format;
  u32 width = 0;
  u32 height = 0;
  u32 data_size = 0; //what will i do for images greater than 4GB?
  u8_ptr data;
};



class MatteLoaderPool;

class MatteLoaderDispatcher : public CharmBase<MatteLoaderDispatcher, MTReferenceCounter>
{
 public:
  enum class LoadStatus
    {
      NotLoading,
      Loading,
      Loaded
    };

  struct LoadJob
  {
    u32 frame_id;
    std::filesystem::path path;
    std::promise<MatteFrameUnique> promise;
  };

  struct LoadResult
  {
    u32 frame_id;
    std::future<MatteFrameUnique> frame;
  };

  MatteLoaderDispatcher ();

  CHARM_DELETE_MOVE_COPY(MatteLoaderDispatcher);

  ~MatteLoaderDispatcher ();

  void SetMatteLoaderPool (MatteLoaderPool *_pool);

  void PushJob (u32 _frame, std::filesystem::path const &_path);
  bool HasJobs ();
  bool PopJob (LoadJob &_job);

  LoadStatus PopFrame (u32 _id, MatteFrameUnique &_frame);
  LoadStatus PopAndReleaseFrames (u32 _id, MatteFrameUnique &_frame);

 protected:
  std::mutex m_job_mutex;
  boost::circular_buffer<LoadJob> m_job_queue;

  std::mutex m_result_mutex;
  boost::circular_buffer<LoadResult> m_result_queue;

  MatteLoaderPool *m_loader_pool;
};

class MatteLoaderPool
{
 public:
  MatteLoaderPool ();
  ~MatteLoaderPool ();

  CHARM_DELETE_MOVE_COPY(MatteLoaderPool);

  void ShutDown ();

  void AddDispatcher (ch_weak_ptr<MatteLoaderDispatcher> const &_dispatcher);
  void AddDispatcher (ch_weak_ptr<MatteLoaderDispatcher> &&_dispatcher);

  void NotifyOfJob ();

  bool NextJob (MatteLoaderDispatcher::LoadJob &_job);

 protected:
  static void LoadMattes (MatteLoaderPool *pool);

  std::atomic_bool m_currently_loading;
  std::vector<std::thread> m_loading_threads;

  std::mutex m_dispatcher_mutex;
  std::condition_variable m_dispatcher_cond_var;
  std::vector<ch_weak_ptr<MatteLoaderDispatcher>> m_dispatchers;
  u64 m_dispatcher_current_index;
};

}

#endif //MATTE_LOADER
