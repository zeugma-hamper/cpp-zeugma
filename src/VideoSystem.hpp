#ifndef BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH
#define BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH

#include <bgfx_utils.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <MatteLoaderPool.hpp>

#include <boost/signals2/signal.hpp>

#include <filesystem>
#include <memory>
#include <string_view>
#include <string>
#include <vector>

#include <gst/gst.h>
#include <gst/video/video-info.h>

namespace charm
{

class VideoRenderable;
class MattedVideoRenderable;
class TampVideoTerminus;
class VideoSystem;
struct DecodePipeline;

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

enum class MediaStatus
{
  Void = 0, //shouldn't happen
  Null, //shouldn't happen
  Ready,
  Paused,
  Playing
};

class VideoTexture : public CharmBase<VideoTexture>
{
 public:
  VideoTexture (VideoFormat _format, bgfx::ProgramHandle _program,
                bgfx::UniformHandle *_unis, size_t _uni_count);

  ~VideoTexture ();

  void BindGraphics (u64 _additional_state, bool _use_matte = true);

  void SetDimensions (v2i32 _dim);
  v2i32 GetDimensions () const;

  void SetMatteDimensions (v2u32 _min, v2u32 _max);
  v2u32 GetMatteMin () const;
  v2u32 GetMatteMax () const;

  void SetNthTexture (size_t _index, bgfx::TextureHandle _handle);
  bgfx::TextureHandle GetNthTexture (size_t _index) const;
  bgfx::TextureHandle &GetNthTexture (size_t _index);

  bgfx::UniformHandle const &GetDimensionUniform () const;
  bgfx::UniformHandle const &GetMatteDimUniform () const;
  bgfx::UniformHandle const &GetFlagsUniform () const;
  bgfx::UniformHandle const &GetOverUniform () const;
  bgfx::UniformHandle const &GetUpUniform () const;
  bgfx::UniformHandle const &GetMixColorUniform () const;
  bgfx::UniformHandle const &GetAdjColorUniform () const;
  bgfx::ProgramHandle const &GetProgram () const;

 protected:

  VideoFormat format;
  v2i32 dimensions = {-1, -1};
  v2u32 matte_min = {0, 0};
  v2u32 matte_max = {0, 0};
  bgfx::ProgramHandle program  = BGFX_INVALID_HANDLE;
  bgfx::UniformHandle uniforms[11] = {BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE,
                                      BGFX_INVALID_HANDLE};
  //at most 4 for Y, U, V, A/matte
  //this is the only graphic resource VideoTexture owns,
  //everything else is owned by VideoSystem
  bgfx::TextureHandle textures[4] = {BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE,
                                     BGFX_INVALID_HANDLE};
};

namespace fs = std::filesystem;

struct MattePipeline
{
  fs::path dir_path;
  f64 start_timestamp = -1.0;
  f64 end_timestamp = -1.0;
  i32 frame_count = 0;
  i32 awaited = -1;
  v2u32 roi_min {0, 0};
  v2u32 roi_max {0, 0};
  MatteFrameUnique last_popped_frame;
  ch_ptr<MatteLoaderWorker> worker;
};

struct ManualTrickPlayState
{
  bool ClearIsReady ();

  bool IsInProgress ();

  void ConditionalSetReady ();

  void Clear ();

  f64 Reset (f64 _pts, f64 _current_ts, f64 _num_steps);

  struct UpdateReturn
  {
    f64 ts = -1.0;
    bool is_finished = false;
  };
  UpdateReturn ConditionalUpdateNextPTS ();

  // call below methods with mutex
  bool ClearIsReadyInternal ();
  bool IsInProgressInternal ();
  void ClearInternal ();
  f64 ResetInternal (f64 _pts, f64 _current_ts, f64 _num_steps);

  std::mutex m_state_mutex;
  f64 m_seek_pts = -1.0;
  f64 m_seek_dist = -1.0;
  f64 m_seek_last_pts = -1.0;
  bool m_is_ready = false;
};

class VideoPipeline : public CharmBase<VideoPipeline>
{
 public:
  VideoPipeline ();
  ~VideoPipeline ();

  CHARM_DELETE_MOVE_COPY(VideoPipeline);

  ch_ptr<VideoTexture> OpenFile (std::string_view _path);
  bool AddMatte (f64 _loop_start_ts, f64 _loop_end_ts,
                 i32 _frame_count, fs::path const &_matte_dir,
                 v2u32 _min, v2u32 _max);

  ch_ptr<DecodePipeline> const &GetDecoder ();
  ch_ptr<VideoTexture> GetVideoTexture ();

  void Play ();
  void Pause ();
  void Seek (f64 _ts);

  void SetPlaySpeed (f64 _speed, bool _trick_play = false);

  MediaStatus GetStatus () const;
  MediaStatus GetPendingStatus () const;
  // true if status doesn't equal pending status
  bool IsInFlux () const;

  void Step (u32 _distance);

  // pass _play_speed as something other than 0.0f to change speed
  // of playback when pipeline is next in PLAY state
  void Loop (f64 _from, f64 _to, f32 _play_speed = 0.0f);
  bool IsLooping () const;

  // timestamp of gstreamer's idea of current position, usually the
  // timestamp of the demuxer's last buffer, I think.
  f64 CurrentPosition () const;
  gint64 CurrentPositionNS () const;

  // timestamp of the latest video buffer
  f64 CurrentTimestamp () const;
  gint64 CurrentTimestampNS () const;

  v2i32 CurrentVideoFrameRateAsRatio () const;
  f64 CurrentVideoFrameRate ()  const;

  v2i32 CurrentAudioFrameRateAsRatio () const;

  f64 Duration () const;
  gint64 DurationNanoseconds () const;

  f32 PlaySpeed () const;

  i64 MatteCount () const;
  MattePipeline &NthMatte (i64 _nth);
  void SetActiveMatte (i64 _nth);
  void ClearMattes ();

  void TrickSeekTo (f64 _pts, f64 _duration);

  // called by Terminus from the streaming thread
  void NewBufferCallback (GstBuffer *_buffer, GstVideoInfo *_info,
                          i64 _pts, i64 _frame_number);

  std::string path;
  ch_ptr<DecodePipeline> pipeline;
  TampVideoTerminus *terminus = nullptr;
  ch_weak_ptr<VideoTexture> texture;
  boost::signals2::connection buffer_connection;
  ManualTrickPlayState trick_mode_state;

  std::mutex m_matte_lock; //heh
  std::vector<MattePipeline> mattes;
  i64 active_matte = -1;
  i64 current_matte = -1;
};

struct VideoBrace
{
  ch_ptr<VideoPipeline> control_pipeline;
  ch_ptr<VideoTexture> video_texture;
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

    bgfx::UniformHandle uniforms[11] = {BGFX_INVALID_HANDLE, //video texture 0
                                        BGFX_INVALID_HANDLE, //video texture 1
                                        BGFX_INVALID_HANDLE, //video texture 2
                                        BGFX_INVALID_HANDLE, //video matte
                                        BGFX_INVALID_HANDLE, //dimensions
                                        BGFX_INVALID_HANDLE, //matte dimensions
                                        BGFX_INVALID_HANDLE, //over
                                        BGFX_INVALID_HANDLE, //up
                                        BGFX_INVALID_HANDLE, //flags
                                        BGFX_INVALID_HANDLE, //mix color
                                        BGFX_INVALID_HANDLE};//adj color

    bgfx::TextureHandle black_texture;
    bgfx::TextureHandle white_texture;
  };

  CHARM_DELETE_MOVE_COPY(VideoSystem);

  static bool Initialize ();
  static void ShutDown ();

  static VideoSystem *GetSystem ();

  void UploadFrames ();
  // TODO: all messages are handled internally, but there are messages the
  // user/programmer and application will be interested in.
  void PollMessages ();

  ch_ptr<DecodePipeline> CreateDecodePipeline (std::string_view _uri);
  ch_ptr<VideoTexture> CreateVideoTexture (VideoFormat _format);
  MattePipeline CreateMattePipeline (f64 _loop_start_ts, f64 _loop_end_ts,
                                     i32 _frame_count, fs::path const &_matte_dir,
                                     v2u32 _min, v2u32 _max);



  VideoBrace OpenVideoFile (std::string_view _path);

  // see MattePathPattern in Matte.hpp
  VideoBrace OpenMatte (std::string_view _uri,
                        f64 _loop_start_ts, f64 _loop_end_ts,
                        i32 _frame_count, fs::path const &_matte_dir,
                        v2u32 _min, v2u32 _max);

  VideoBrace DuplicateVideo (ch_ptr<VideoPipeline> const &_pipeline);
  VideoBrace DuplicateMatte (ch_ptr<VideoPipeline> const &_pipeline,
                             i64 _matte_index);

  ch_ptr<VideoPipeline> FindVideoPipeline (ch_ptr<VideoTexture> const &_texture);

  //NOTE: this is internal. you shouldn't need to call this.
  void DestroyVideo (VideoTexture *_texture);


  //access to some generally useful graphics resources, do not free these
  bgfx::TextureHandle GetBlackTexture () const;
  bgfx::TextureHandle GetWhiteTexture () const;

  u64 GetVideoBGFXState () const;
  u64 GetMatteBGFXState () const;

  GraphicsResources &GetGraphicsResources ();

 protected:
  VideoSystem ();
  ~VideoSystem ();


  GraphicsResources m_vgr;
  std::vector<ch_ptr<VideoPipeline>> m_pipelines;
  std::unique_ptr<MatteLoaderPool> m_matte_pool;
  szt m_upload_position;
};

}

#endif  //BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH
