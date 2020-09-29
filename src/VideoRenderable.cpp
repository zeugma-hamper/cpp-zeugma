#include <VideoRenderable.hpp>

#include <charm_glm.hpp>

#include <Node.hpp>
#include <VideoSystem.hpp>

#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <string_view>

namespace charm
{

VideoRenderable::VideoRenderable (std::string_view _uri)
  : Renderable (),
    m_video_texture {nullptr}
{
  VideoSystem *system = VideoSystem::GetSystem();
  m_video_texture = system->OpenVideo (_uri);
}

VideoRenderable::~VideoRenderable ()
{
}

void VideoRenderable::Draw ()
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture (0)))
    return;

  // fprintf (stderr, "drawing\n");

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);

  bgfx::setVertexCount(4);
  // TODO: get a hold of this information
  glm::vec4 unity {1.0f, 1080.0f/1920.0f, 1.0f, 1.0f};
  bgfx::setUniform(m_video_texture->GetAspectUniform(), glm::value_ptr (unity));
  bgfx::submit(0, m_video_texture->GetProgram ());
}

}
