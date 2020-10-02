#ifndef VIDEO_RENDERABLE
#define VIDEO_RENDERABLE

#include <Renderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>
#include <intrusive_ptr.hpp>

namespace charm
{

struct VideoTexture;

class VideoRenderable final : public Renderable
{
 public:

  VideoRenderable (std::string_view _uri);
  ~VideoRenderable () override;

  void Draw () override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
};

}

#endif //VIDEO_RENDERABLE
