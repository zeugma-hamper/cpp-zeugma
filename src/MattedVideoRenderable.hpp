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

struct FilmInfo;
struct ClipInfo;

class MattedVideoRenderable final : public Renderable
{
 public:

  MattedVideoRenderable (std::string_view _uri,
                         f64 _loop_start_ts, f64 _loop_end_ts,
                         std::string_view _matte_pattern);
  MattedVideoRenderable (FilmInfo const &_film, ClipInfo &_clip);
  ~MattedVideoRenderable () override;

  void Draw () override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
};


}

#endif //MATTED_VIDEO_RENDERABLE
