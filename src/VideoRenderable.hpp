#ifndef VIDEO_RENDERABLE
#define VIDEO_RENDERABLE

#include <Renderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>
#include <ch_ptr.hpp>

namespace charm
{

struct VideoTexture;
struct FilmInfo;

class VideoRenderable final : public Renderable
{
 public:

  VideoRenderable (std::string_view _uri);
  VideoRenderable (FilmInfo const &_film);
  ~VideoRenderable ()  override;

  const ch_ptr <VideoTexture> ItsVideoTexture ()
    { return m_video_texture; }

  const ch_ptr <DecodePipeline> ItsDecodePipeline ();

  void Draw (u16 vyu_id) override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
};

}

#endif //VIDEO_RENDERABLE
