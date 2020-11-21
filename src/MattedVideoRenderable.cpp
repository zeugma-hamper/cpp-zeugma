
#include <MattedVideoRenderable.hpp>

#include <BlockTimer.hpp>
#include <Matte.hpp>
#include <Node.hpp>

#include <vector_interop.hpp>

#include <algorithm>


namespace charm  {


namespace fs = std::filesystem;


MattedVideoRenderable::MattedVideoRenderable (std::string_view _uri,
                                              f64 _loop_start_ts,
                                              f64 _loop_end_ts,
                                              std::string_view _matte_pattern)
  : Renderable ()
{
  VideoBrace brace = VideoSystem::GetSystem ()
    -> OpenMatte (_uri, _loop_start_ts, _loop_end_ts, -1, _matte_pattern,
                  v2i32{-1, -1}, v2i32{-1, -1});
  m_video_texture = brace.video_texture;
  m_uni_adj_iro = bgfx::createUniform ("u_adjc", bgfx::UniformType::Vec4);
}


MattedVideoRenderable::MattedVideoRenderable (FilmInfo const &_film,
                                              ClipInfo const &_clip)
  : Renderable ()
{
  v2i32 min = {i32 (_clip.geometry.dir_geometry.min[0]),
               i32 (_clip.geometry.dir_geometry.min[1])};
  v2i32 max = {i32 (_clip.geometry.dir_geometry.max[0]),
               i32 (_clip.geometry.dir_geometry.max[1])};

  VideoBrace brace = VideoSystem::GetSystem ()
    -> OpenMatte (std::string ("file://") + _film.film_path.string(),
                  _clip.start_time, _clip.start_time + _clip.duration,
                  _clip.frame_count, _clip.directory, min, max);
  m_video_texture = brace.video_texture;
  m_uni_adj_iro = bgfx::createUniform("u_adjc", bgfx::UniformType::Vec4);
}


MattedVideoRenderable::~MattedVideoRenderable ()
{ bgfx::destroy (m_uni_adj_iro); }


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


void MattedVideoRenderable::Draw (u16 vyu_id)
{
  if (! m_video_texture
      || ! bgfx::isValid(m_video_texture->GetNthTexture(0))
      || ! bgfx::isValid(m_video_texture->GetNthTexture(3)))
    return;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture
    -> BindGraphics (BGFX_STATE_PT_TRISTRIP  |
                     BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                            BGFX_STATE_BLEND_INV_SRC_ALPHA));
  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  v2i32 const matte_min = m_video_texture->GetMatteMin();
  v2i32 const matte_max = m_video_texture->GetMatteMax();

  glm::vec4 vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  glm::vec4 matte_dim {matte_min.x, matte_min.y, matte_max.x, matte_max.y};

  bgfx::setUniform (m_video_texture->GetDimensionUniform(),
                    glm::value_ptr (vid_dim));
  bgfx::setUniform (m_video_texture->GetMatteDimUniform(),
                    glm::value_ptr (matte_dim));

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  glm::vec4 const ac = as_glm (m_adjc.val);

  bgfx::setUniform (m_video_texture->GetOverUniform(), &over);
  bgfx::setUniform (m_video_texture->GetUpUniform(), &up);
  bgfx::setUniform (m_uni_adj_iro, &ac);

  bgfx::submit(vyu_id, m_video_texture->GetProgram());
}


}
