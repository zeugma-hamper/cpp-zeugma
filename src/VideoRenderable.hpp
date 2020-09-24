#ifndef VIDEO_RENDERABLE
#define VIDEO_RENDERABLE

#include <Renderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>

namespace charm
{

class VideoRenderable final : public Renderable
{
 public:

  VideoRenderable (std::string_view _uri);
  ~VideoRenderable () override;

  void UploadSample (gst_ptr<GstSample> const &_sample);

  void Update () override;

  void Draw () override;

 private:
  bgfx::ProgramHandle m_program;
  bgfx::TextureHandle m_texture;
  bgfx::UniformHandle m_uni_vid_texture;
  bgfx::UniformHandle m_uni_aspect_ratio;
  DecodePipeline *m_video_pipeline;
  BasicPipelineTerminus *m_terminus;
  f32 m_aspect_ratio;
};

}

#endif //VIDEO_RENDERABLE
