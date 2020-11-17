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

enum class SizeReferent
{
  Matte = 0,
  Video
};

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

  void SetEnableMatte (bool _tf);
  bool GetEnableMatte () const;

  void EnableMatte ();
  void DisableMatte ();

  // Matte clips and scales to the matte's region of interest
  // Video scales matte to video size and aspect
  void SetSizeReferent (SizeReferent _ref);
  SizeReferent GetSizeReferent () const;

  void Draw (u16 vyu_id) override;

 private:
  ch_ptr<VideoTexture> m_video_texture;
  u64 m_bgfx_state;
  SizeReferent m_size_referent;
  bool m_enable_matte;
};


}

#endif //MATTED_VIDEO_RENDERABLE
