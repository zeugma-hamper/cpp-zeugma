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
    ->OpenMatte (_uri, _loop_start_ts, _loop_end_ts, -1, _matte_pattern);
  m_video_texture = brace.video_texture;
}

MattedVideoRenderable::MattedVideoRenderable (FilmInfo const &_film, ClipInfo &_clip)
  : Renderable ()
{

  VideoBrace brace = VideoSystem::GetSystem ()
    ->OpenMatte (std::string ("file://") + _film.film_path.string(),
                 _clip.start_time, _clip.start_time + _clip.duration,
                 _clip.frame_count, _clip.directory);
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
  f32 inv_aspect = f32(dim.y)/dim.x;
  glm::vec4 unity {1.0f, inv_aspect, 1.0f, 1.0f};
  bgfx::setUniform(m_video_texture->GetAspectUniform(), glm::value_ptr (unity));

  bgfx::submit(vyu_id, m_video_texture->GetProgram());
}

}
