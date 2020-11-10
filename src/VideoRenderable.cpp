
#include <VideoRenderable.hpp>

#include <charm_glm.hpp>
#include <vector_interop.hpp>

#include <Node.hpp>
#include <VideoSystem.hpp>
#include <Matte.hpp>

#include <unistd.h>

#include <string>
#include <string_view>



namespace charm
{

VideoRenderable::VideoRenderable (std::string_view _uri)
  : Renderable (),
    m_video_texture {nullptr}
{
  VideoSystem *system = VideoSystem::GetSystem();

  VideoBrace brace = system->OpenVideo (_uri);
  m_video_texture = brace.video_texture;
}

VideoRenderable::VideoRenderable (FilmInfo const &_fm)
  : Renderable (),
    m_video_texture {nullptr}
{
  VideoSystem *system = VideoSystem::GetSystem();

  VideoBrace brace = system->OpenVideo (std::string ("file://") + _fm.film_path.string ());
  m_video_texture = brace.video_texture;
}

VideoRenderable::~VideoRenderable ()
{
}


const ch_ptr <DecodePipeline> VideoRenderable::ItsDecodePipeline ()
{ return VideoSystem::GetSystem ()
    -> FindDecodePipeline (m_video_texture);
}


void VideoRenderable::Draw (u16 vyu_id)
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture (0)))
    return;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);

  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  glm::vec4 vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  bgfx::setUniform(m_video_texture->GetDimensionUniform (), glm::value_ptr (vid_dim));

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  bgfx::setUniform(m_video_texture->GetOverUniform(), &over);
  bgfx::setUniform(m_video_texture->GetUpUniform(), &up);

  bgfx::submit(vyu_id, m_video_texture->GetProgram ());
}

}
