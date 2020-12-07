
#include <MattedVideoRenderable.hpp>

#include <BlockTimer.hpp>
#include <Matte.hpp>
#include <Node.hpp>

#include <vector_interop.hpp>

#include <algorithm>


namespace charm  {


namespace fs = std::filesystem;

MattedVideoRenderable::MattedVideoRenderable ()
  : Renderable (),
    m_bgfx_state {0},
    m_size_referent {SizeReferent::Matte},
    m_enable_matte {true},
    m_enable_mix_color {false},
    m_mix_color {0.275, 0.275, 0.275, 1.0f}
{
  EnableMatte();
}

MattedVideoRenderable::MattedVideoRenderable (ch_ptr<VideoTexture> const &_texture)
  : MattedVideoRenderable ()
{
  m_video_texture = _texture;
}

MattedVideoRenderable::MattedVideoRenderable (std::string_view _path,
                                              f64 _loop_start_ts, f64 _loop_end_ts,
                                              std::string_view _matte_pattern)
  : MattedVideoRenderable ()
{
  VideoSystem *system = VideoSystem::GetSystem ();
  assert (system);

  VideoBrace brace
    = system->OpenMatte (_path, _loop_start_ts, _loop_end_ts,
                         -1, _matte_pattern,
                         v2u32{0, 0}, v2u32{0, 0});

  m_video_texture = brace.video_texture;
  m_bgfx_state = system->GetMatteBGFXState() | BGFX_STATE_PT_TRISTRIP;
}

MattedVideoRenderable::MattedVideoRenderable (FilmInfo const &_film, ClipInfo const &_clip)
  : MattedVideoRenderable ()
{
  VideoSystem *system = VideoSystem::GetSystem ();
  assert (system);

  VideoBrace brace
    = system->OpenMatte (_film.film_path.string(),
                         _clip.start_time, _clip.start_time + _clip.duration,
                         _clip.frame_count, _clip.directory,
                         _clip.geometry.dir_geometry.min, _clip.geometry.dir_geometry.max);

  m_video_texture = brace.video_texture;
  m_bgfx_state = system->GetMatteBGFXState() | BGFX_STATE_PT_TRISTRIP;
}


MattedVideoRenderable::~MattedVideoRenderable ()
{ }


ch_ptr<VideoTexture> const &MattedVideoRenderable::GetVideoTexture () const
{
  return m_video_texture;
}

ch_ptr<VideoPipeline> MattedVideoRenderable::GetVideoPipeline () const
{
  if (auto *system = VideoSystem::GetSystem(); system)
    return system->FindVideoPipeline(GetVideoTexture ());

  return {};
}

v2i32 MattedVideoRenderable::GetVideoDimensions ()  const
{
  if (m_video_texture)
    return m_video_texture -> GetDimensions ();
  return { -1, -1 };
}

MattedVideoRenderable *MattedVideoRenderable::DuplicateVideoOnly () const
{
  auto *system = VideoSystem::GetSystem();
  if (!system)
    return nullptr;

  ch_ptr<VideoPipeline> pipeline = system->FindVideoPipeline(m_video_texture);
  VideoBrace brace = system->DuplicateVideo(pipeline);
  if (! brace.video_texture)
    return nullptr;

  MattedVideoRenderable *mvr = new MattedVideoRenderable;
  mvr->m_bgfx_state = system->GetGraphicsResources().basic_state;
  mvr->m_size_referent = SizeReferent::Video;
  mvr->m_enable_matte = false;
  mvr->m_video_texture = brace.video_texture;

  return mvr;
}

MattedVideoRenderable *MattedVideoRenderable::DuplicateWithActiveMatte () const
{
  auto *system = VideoSystem::GetSystem();
  if (!system)
    return nullptr;

  ch_ptr<VideoPipeline> pipeline = system->FindVideoPipeline(m_video_texture);
  return DuplicateWithMatte (pipeline->active_matte);
}

MattedVideoRenderable *MattedVideoRenderable::DuplicateWithMatte (i64 _matted_index) const
{
  auto *system = VideoSystem::GetSystem();
  if (!system)
    return nullptr;

  ch_ptr<VideoPipeline> pipeline = system->FindVideoPipeline(m_video_texture);
  VideoBrace brace = system->DuplicateMatte(pipeline, _matted_index);
  if (! brace.video_texture)
    return nullptr;

  MattedVideoRenderable *mvr = new MattedVideoRenderable;
  mvr->m_bgfx_state = system->GetGraphicsResources().matte_state;
  mvr->m_size_referent = m_size_referent;
  mvr->m_enable_matte = m_enable_matte;
  mvr->m_video_texture = brace.video_texture;

  return mvr;
}


void MattedVideoRenderable::SetEnableMatte (bool _tf)
{
  m_enable_matte = _tf;

  VideoSystem *system = VideoSystem::GetSystem ();
  assert (system);
  u64 const state = m_enable_matte ? system->GetMatteBGFXState() : system->GetVideoBGFXState();
  m_bgfx_state = state | BGFX_STATE_PT_TRISTRIP;
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

void MattedVideoRenderable::SetEnableMixColor (bool _tf)
{
  m_enable_mix_color = _tf;
}

bool MattedVideoRenderable::GetEnableMixColor () const
{
  return m_enable_mix_color;
}

void MattedVideoRenderable::SetMixColor (glm::vec4 const &_color)
{
  m_mix_color = _color;
}

glm::vec4 MattedVideoRenderable::GetMixColor () const
{
  return m_mix_color;
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
  if (! m_video_texture
      || ! bgfx::isValid(m_video_texture->GetNthTexture(0))
      || (GetEnableMatte()
          && ! bgfx::isValid(m_video_texture->GetNthTexture(3))))
    return;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics
    (m_bgfx_state  |
     BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                            BGFX_STATE_BLEND_INV_SRC_ALPHA),
     GetEnableMatte ());

  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  v2u32 const matte_min = m_video_texture->GetMatteMin();
  v2u32 const matte_max = m_video_texture->GetMatteMax();

  glm::vec4 const vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  glm::vec4 const matte_dim {matte_min.x, matte_min.y, matte_max.x, matte_max.y};

  bgfx::setUniform (m_video_texture->GetDimensionUniform(),
                    glm::value_ptr (vid_dim));
  bgfx::setUniform (m_video_texture->GetMatteDimUniform(),
                    glm::value_ptr (matte_dim));

  glm::vec4 const flags {f32 (m_size_referent),
                         GetEnableMixColor() ? 1.0f : 0.0f, 0.0f, 0.0f};
  bgfx::setUniform(m_video_texture->GetFlagsUniform(), glm::value_ptr (flags));
  bgfx::setUniform(m_video_texture->GetMixColorUniform(), glm::value_ptr (m_mix_color));

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  glm::vec4 const ac = as_glm (m_adjc.val);

  bgfx::setUniform (m_video_texture->GetOverUniform(), &over);
  bgfx::setUniform (m_video_texture->GetUpUniform(), &up);
  bgfx::setUniform (m_video_texture->GetAdjColorUniform(), glm::value_ptr (ac));

  bgfx::submit(vyu_id, m_video_texture->GetProgram());
}


}
