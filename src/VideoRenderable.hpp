
//
// (c) treadle & loam, provisioners llc
//

#ifndef VIDEO_RENDERABLE
#define VIDEO_RENDERABLE


#include <Renderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>
#include <ch_ptr.hpp>


namespace zeugma  {


struct VideoTexture;
struct FilmInfo;


class VideoRenderable final : public Renderable
{
 public:

  VideoRenderable (std::string_view _path);
  VideoRenderable (FilmInfo const &_film);
  ~VideoRenderable ()  override;

  ch_ptr<VideoTexture> const &GetVideoTexture () const;
  ch_ptr<VideoPipeline> GetVideoPipeline () const;

  void Draw (u16 vyu_id) override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
  u64 m_bgfx_state;
};


}


#endif  //VIDEO_RENDERABLE
