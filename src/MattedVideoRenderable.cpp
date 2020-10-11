#include <MattedVideoRenderable.hpp>

#include <BlockTimer.hpp>
#include <Matte.hpp>
#include <Node.hpp>

#include <algorithm>

namespace charm
{

namespace fs = std::filesystem;
MattedVideoRenderable::MattedVideoRenderable (std::string_view _uri,
                                              f64 _loop_start_ts, f64 _loop_end_ts,
                                              std::string_view _matte_pattern)
  : Renderable ()
{
  VideoBrace brace = VideoSystem::GetSystem ()
    ->OpenMatte (_uri, _loop_start_ts, _loop_end_ts, -1, _matte_pattern,
                 v2i32{-1, -1}, v2i32{-1, -1});
  m_video_texture = brace.video_texture;
}

MattedVideoRenderable::MattedVideoRenderable (FilmInfo const &_film, ClipInfo &_clip)
  : Renderable ()
{
  v2i32 min = {i32 (_clip.geometry.dir_geometry.min[0]),i32 (_clip.geometry.dir_geometry.min[1])};
  v2i32 max = {i32 (_clip.geometry.dir_geometry.max[0]),i32 (_clip.geometry.dir_geometry.max[1])};

  VideoBrace brace = VideoSystem::GetSystem ()
    ->OpenMatte (std::string ("file://") + _film.film_path.string(),
                 _clip.start_time, _clip.start_time + _clip.duration,
                 _clip.frame_count, _clip.directory, min, max);
  m_video_texture = brace.video_texture;
}

MattedVideoRenderable::~MattedVideoRenderable ()
{
}

void MattedVideoRenderable::Draw (u16 vyu_id)
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture(0)))
    return;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);
  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  v2i32 const matte_min = m_video_texture->GetMatteMin();
  v2i32 const matte_max = m_video_texture->GetMatteMax();

  glm::vec4 vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  glm::vec4 matte_dim {matte_min.x, matte_min.y, matte_max.x, matte_max.y};

  bgfx::setUniform(m_video_texture->GetDimensionUniform(), glm::value_ptr (vid_dim));
  bgfx::setUniform(m_video_texture->GetMatteDimUniform(),  glm::value_ptr (matte_dim));

  bgfx::submit(vyu_id, m_video_texture->GetProgram());
}

}
