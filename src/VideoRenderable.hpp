
#ifndef VIDEO_RENDERABLE
#define VIDEO_RENDERABLE

#include <Renderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>
#include <ch_ptr.hpp>


namespace charm  {


struct VideoTexture;
struct FilmInfo;


class VideoRenderable final : public Renderable
{
 public:

  VideoRenderable (std::string_view _uri);
  VideoRenderable (FilmInfo const &_film);
  ~VideoRenderable ()  override;

  ch_ptr<VideoTexture> const &GetVideoTexture () const;
  ch_ptr<DecodePipeline> GetPipeline () const;

  void Draw (u16 vyu_id) override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
  bgfx::UniformHandle m_uni_adj_iro;
};


}


#endif //VIDEO_RENDERABLE
