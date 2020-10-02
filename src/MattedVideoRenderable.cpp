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
                                              std::filesystem::path const &_matte_dir)
  : Renderable ()
{
  m_video_texture = VideoSystem::GetSystem ()
    ->OpenMatte (_uri, _loop_start_ts, _loop_end_ts, _matte_dir);
}

MattedVideoRenderable::MattedVideoRenderable (FilmInfo const &_film, ClipInfo &_clip)
  : Renderable ()
{
  m_video_texture = VideoSystem::GetSystem ()
    ->OpenMatte (std::string ("file://") + _film.film_path.string(),
                 _clip.start_time, _clip.start_time + _clip.duration,
                 _clip.directory);

}
MattedVideoRenderable::~MattedVideoRenderable ()
{
}

void MattedVideoRenderable::Draw ()
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture(0)))
    return;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);
  bgfx::setVertexCount(4);

  glm::vec4 unity {1.0f, 1080.0f/1920.0f, 1.0f, 1.0f};
  bgfx::setUniform(m_video_texture->GetAspectUniform(), glm::value_ptr (unity));

  bgfx::submit(0, m_video_texture->GetProgram());
}

}
