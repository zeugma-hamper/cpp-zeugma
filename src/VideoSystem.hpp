#ifndef BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH
#define BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH

// #include <DecodePipeline.hpp>
// #include <PipelineTerminus.hpp>
// #include <VideoRenderable.hpp>
// #include <MattedVideoRenderable.hpp>

#include <bgfx_utils.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <filesystem>
#include <string_view>
#include <string>
#include <vector>

namespace charm
{

class VideoRenderable;
class MattedVideoRenderable;
class DecodePipeline;
class BasicPipelineTerminus;
class VideoSystem;

//sketch

enum class VideoComponent
{
  RGB = 0,
  Y = 0,
  U = 1,
  UV = 1,
  V = 2,
  A = 3
};

enum class VideoFormat
{
  RGB = 0,
  I420 = 1,
};

class VideoTexture : public CharmBase<VideoTexture>
{
 public:
  VideoTexture (VideoFormat _format, u64 _state, bgfx::ProgramHandle _program,
                bgfx::UniformHandle *_unis, size_t _uni_count);

  ~VideoTexture ();

  u64 GetDefaultState () const;

  void BindGraphics (u64 _additional_state);

  void SetNthTexture (size_t _index, bgfx::TextureHandle _handle);
  bgfx::TextureHandle GetNthTexture (size_t _index) const;

  bgfx::UniformHandle const &GetAspectUniform () const;
  bgfx::ProgramHandle const &GetProgram () const;

 protected:

  VideoFormat format;
  u64 const default_state = 0u;
  bgfx::ProgramHandle program  = BGFX_INVALID_HANDLE;
  bgfx::UniformHandle uniforms[5] = {BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE};
  //at most 4 for Y, U, V, A/matte
  bgfx::TextureHandle textures[4] = {BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE};
};

namespace fs = std::filesystem;

struct VideoPipeline
{
  std::string uri;
  DecodePipeline *pipeline = nullptr;
  BasicPipelineTerminus *terminus = nullptr;
  ch_weak_ptr<VideoTexture> texture;
  //for mattes
  f64 loop_start_ts = -1.0;
  f64 loop_end_ts = -1.0;
  fs::path matte_dir_path;
  std::vector<fs::directory_entry> matte_file_paths;
};


class VideoSystem
{
 public:
  struct GraphicsResources
  {
    u64 const basic_state =  BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z;
    bgfx::ProgramHandle basic_program = BGFX_INVALID_HANDLE;

    u64 const matte_state = BGFX_STATE_WRITE_RGB |
                            BGFX_STATE_WRITE_A   |
                            BGFX_STATE_WRITE_Z   |
                            BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                                                  BGFX_STATE_BLEND_INV_SRC_ALPHA);
    bgfx::ProgramHandle matte_program = BGFX_INVALID_HANDLE;

    bgfx::UniformHandle uniforms[5] = {BGFX_INVALID_HANDLE,
                                       BGFX_INVALID_HANDLE,
                                       BGFX_INVALID_HANDLE,
                                       BGFX_INVALID_HANDLE,
                                       BGFX_INVALID_HANDLE};
  };

  CHARM_DELETE_MOVE_COPY(VideoSystem);

  static bool Initialize ();
  static void ShutDown ();

  static VideoSystem *GetSystem ();

  void UploadFrames ();
  // TODO: all messages are handled internally, but there are messages the
  // user/programmer and application will be interested in.
  void PollMessages ();

  ch_ptr<VideoTexture> OpenVideo (std::string_view _uri);
  void DestroyVideo (VideoTexture *_texture);

  ch_ptr<VideoTexture> OpenMatte (std::string_view _uri,
                                         f64 _loop_start_ts, f64 _loop_end_ts,
                                         std::filesystem::path const &_matte_dir);

 protected:
  VideoSystem ();
  ~VideoSystem ();


  GraphicsResources m_vgr;
  std::vector<VideoPipeline> m_pipelines;
};

}

#endif  //BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH
