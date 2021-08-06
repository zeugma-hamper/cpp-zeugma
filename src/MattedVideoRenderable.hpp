
#ifndef MATTED_VIDEO_RENDERABLE
#define MATTED_VIDEO_RENDERABLE


#include <Renderable.hpp>
#include <VideoSystem.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <bgfx_utils.hpp>

#include <filesystem>


namespace zeugma  {


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

  MattedVideoRenderable (ch_ptr<VideoTexture> const &_texture);
  MattedVideoRenderable (std::string_view _uri,
                         f64 _loop_start_ts, f64 _loop_end_ts,
                         std::string_view _matte_pattern);
  MattedVideoRenderable (FilmInfo const &_film, ClipInfo const &_clip);
  ~MattedVideoRenderable () override;

  ch_ptr<VideoTexture> const &GetVideoTexture () const;
  ch_ptr<VideoPipeline> GetVideoPipeline () const;

  v2i32 GetVideoDimensions ()  const;

  MattedVideoRenderable *DuplicateVideoOnly () const;
  MattedVideoRenderable *DuplicateWithActiveMatte () const;
  MattedVideoRenderable *DuplicateWithMatte (i64 _matted_index) const;

  void SetEnableMatte (bool _tf);
  bool GetEnableMatte () const;

  void EnableMatte ();
  void DisableMatte ();

  void SetEnableMixColor (bool _tf);
  bool GetEnableMixColor () const;

  void SetMixColor (glm::vec4 const &_color);
  glm::vec4 GetMixColor () const;

  // Matte clips and scales to the matte's region of interest
  // Video scales matte to video size and aspect
  void SetSizeReferent (SizeReferent _ref);
  SizeReferent GetSizeReferent () const;

  void Draw (u16 vyu_id) override;

 private:

  MattedVideoRenderable ();

  ch_ptr<VideoTexture> m_video_texture;
  u64 m_bgfx_state;
  SizeReferent m_size_referent;
  bool m_enable_matte;
  bool m_enable_mix_color;
  glm::vec4 m_mix_color;
};


}


#endif  //MATTED_VIDEO_RENDERABLE
