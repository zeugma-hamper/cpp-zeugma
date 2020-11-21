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
  MattedVideoRenderable (FilmInfo const &_film, ClipInfo const &_clip);
  ~MattedVideoRenderable () override;

  ch_ptr<VideoTexture> const &GetVideoTexture () const;
  ch_ptr<DecodePipeline> GetPipeline () const;

  void Draw (u16 vyu_id) override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
  bgfx::UniformHandle m_uni_adj_iro;
};


}

#endif //MATTED_VIDEO_RENDERABLE
