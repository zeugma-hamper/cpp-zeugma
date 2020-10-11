#include <VideoRenderable.hpp>

#include <charm_glm.hpp>

#include <Node.hpp>
#include <VideoSystem.hpp>

#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <string_view>
#include <type_int.hpp>

namespace charm
{

VideoRenderable::VideoRenderable (std::string_view _uri)
  : Renderable (),
    m_video_texture {nullptr}
{
  VideoSystem *system = VideoSystem::GetSystem();

  VideoBrace brace = system->OpenVideo (_uri);
  m_video_texture = brace.video_texture;
  fprintf (stderr, "renderable's index is %u\n", index<Renderable>::get ());
}

VideoRenderable::~VideoRenderable ()
{
}

void VideoRenderable::Draw (u16 vyu_id)
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture (0)))
    return;

  // fprintf (stderr, "drawing\n");

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);

  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  glm::vec4 unity {dim.x, dim.y, 1.0f, 1.0f};
  bgfx::setUniform(m_video_texture->GetDimensionUniform (), glm::value_ptr (unity));
  bgfx::submit(vyu_id, m_video_texture->GetProgram ());
}

}
