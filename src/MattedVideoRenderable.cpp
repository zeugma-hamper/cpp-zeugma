#include <MattedVideoRenderable.hpp>

#include <BlockTimer.hpp>
#include <Matte.hpp>
#include <Node.hpp>

#include <vector_interop.hpp>

#include <algorithm>

namespace charm
{

namespace fs = std::filesystem;
MattedVideoRenderable::MattedVideoRenderable (std::string_view _path,
                                              f64 _loop_start_ts, f64 _loop_end_ts,
                                              std::string_view _matte_pattern)
  : Renderable (),
    m_size_referent {SizeReferent::Matte},
    m_enable_matte {true}
{
  VideoSystem *system = VideoSystem::GetSystem ();
  assert (system);

  VideoBrace brace
    = system->OpenMatte (_path, _loop_start_ts, _loop_end_ts,
                         -1, _matte_pattern,
                         v2i32{-1, -1}, v2i32{-1, -1});
  m_video_texture = brace.video_texture;
  m_bgfx_state = system->GetMatteBGFXState() | BGFX_STATE_PT_TRISTRIP;
}

MattedVideoRenderable::MattedVideoRenderable (FilmInfo const &_film, ClipInfo const &_clip)
  : Renderable (),
    m_size_referent {SizeReferent::Matte},
    m_enable_matte {true}
{
  VideoSystem *system = VideoSystem::GetSystem ();
  assert (system);

  v2i32 min = {i32 (_clip.geometry.dir_geometry.min[0]),
    i32 (_clip.geometry.dir_geometry.min[1])};
  v2i32 max = {i32 (_clip.geometry.dir_geometry.max[0]),
    i32 (_clip.geometry.dir_geometry.max[1])};

  VideoBrace brace
    = system->OpenMatte (_film.film_path.string(),
                         _clip.start_time, _clip.start_time + _clip.duration,
                         _clip.frame_count, _clip.directory, min, max);
  m_video_texture = brace.video_texture;
  m_bgfx_state = system->GetMatteBGFXState() | BGFX_STATE_PT_TRISTRIP;
}

MattedVideoRenderable::~MattedVideoRenderable ()
{
}

ch_ptr<VideoTexture> const &MattedVideoRenderable::GetVideoTexture () const
{
  return m_video_texture;
}

ch_ptr<DecodePipeline> MattedVideoRenderable::GetPipeline () const
{
  if (auto *system = VideoSystem::GetSystem(); system)
    return system->FindDecodePipeline(GetVideoTexture ());

  return {};
}

void MattedVideoRenderable::SetEnableMatte (bool _tf)
{
  m_enable_matte = _tf;
}

bool MattedVideoRenderable::GetEnableMatte () const
{
  return m_enable_matte;
}

void MattedVideoRenderable::EnableMatte ()
{
  SetEnableMatte(true);
}

void MattedVideoRenderable::DisableMatte ()
{
  SetEnableMatte(false);
}

void MattedVideoRenderable::SetSizeReferent (SizeReferent _ref)
{
  m_size_referent = _ref;
}

SizeReferent MattedVideoRenderable::GetSizeReferent () const
{
  return m_size_referent;
}

void MattedVideoRenderable::Draw (u16 vyu_id)
{
  fprintf (stderr, "mvr: in draw\n");
  if (! m_video_texture
      || ! bgfx::isValid(m_video_texture->GetNthTexture(0))
      || (GetEnableMatte()
          && ! bgfx::isValid(m_video_texture->GetNthTexture(3))))
    return;

  fprintf (stderr, "mvr: after sanity check\n");

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (m_bgfx_state, GetEnableMatte());
  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  v2i32 const matte_min = m_video_texture->GetMatteMin();
  v2i32 const matte_max = m_video_texture->GetMatteMax();

  glm::vec4 const vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  glm::vec4 const matte_dim {matte_min.x, matte_min.y, matte_max.x, matte_max.y};

  bgfx::setUniform(m_video_texture->GetDimensionUniform(), glm::value_ptr (vid_dim));
  bgfx::setUniform(m_video_texture->GetMatteDimUniform(),  glm::value_ptr (matte_dim));

  glm::vec4 const flags {f32 (m_size_referent), 0.0f, 0.0f, 0.0f};
  bgfx::setUniform(m_video_texture->GetFlagsUniform(), glm::value_ptr (flags));

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  bgfx::setUniform(m_video_texture->GetOverUniform(), &over);
  bgfx::setUniform(m_video_texture->GetUpUniform(), &up);

  bgfx::submit(vyu_id, m_video_texture->GetProgram());
}

}
