#ifndef MATTED_VIDEO_RENDERABLE
#define MATTED_VIDEO_RENDERABLE

#include <Renderable.hpp>

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

  void UploadSample (gst_ptr<GstSample> const &_sample);

  void Update () override;

  void Draw () override;

 private:
  bgfx::ProgramHandle m_program;
  bgfx::TextureHandle m_texture;
  bgfx::TextureHandle m_matte;
  bgfx::UniformHandle m_uni_vid_texture;
  bgfx::UniformHandle m_uni_matte_texture;
  bgfx::UniformHandle m_uni_aspect_ratio;

  DecodePipeline *m_video_pipeline;
  BasicPipelineTerminus *m_terminus;
  f32 m_aspect_ratio;

  std::filesystem::path m_matte_path;
  std::vector<std::filesystem::directory_entry> m_mattes;
};


}

#endif //MATTED_VIDEO_RENDERABLE
