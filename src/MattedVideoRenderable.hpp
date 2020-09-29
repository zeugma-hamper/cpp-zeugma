#ifndef MATTED_VIDEO_RENDERABLE
#define MATTED_VIDEO_RENDERABLE

#include <Renderable.hpp>
#include <VideoSystem.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>

#include <filesystem>

namespace charm
{

class MattedVideoRenderable final : public Renderable
{
 public:

  MattedVideoRenderable (std::string_view _uri,
                         f64 _loop_start_ts, f64 _loop_end_ts,
                         std::filesystem::path const &_matte_dir);
  ~MattedVideoRenderable () override;

  // void UploadSample (gst_ptr<GstSample> const &_sample);

  // void Update () override;

  void Draw () override;

 private:
  intrusive_ptr<VideoTexture> m_video_texture;
};


}

#endif //MATTED_VIDEO_RENDERABLE
