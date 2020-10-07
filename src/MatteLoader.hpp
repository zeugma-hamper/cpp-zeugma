#ifndef MATTE_LOADER
#define MATTE_LOADER

#include <base_types.hpp>
#include <bgfx_utils.hpp>
#include <class_utils.hpp>

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

}

#endif //MATTE_LOADER
