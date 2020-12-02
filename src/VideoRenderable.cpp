
#include <VideoRenderable.hpp>

#include <charm_glm.hpp>
#include <vector_interop.hpp>

#include <Node.hpp>
#include <VideoSystem.hpp>
#include <Matte.hpp>

#include <unistd.h>

#include <string>
#include <string_view>



namespace charm  {


VideoRenderable::VideoRenderable (std::string_view _path)
  : Renderable (),
    m_video_texture {nullptr}
{
  VideoSystem *system = VideoSystem::GetSystem();

  VideoBrace brace = system->OpenVideoFile (_path);
  m_video_texture = brace.video_texture;
  m_bgfx_state = system->GetVideoBGFXState() | BGFX_STATE_PT_TRISTRIP;
}

VideoRenderable::VideoRenderable (FilmInfo const &_fm)
  : Renderable (),
    m_video_texture {nullptr}
{
  VideoSystem *system = VideoSystem::GetSystem();

  VideoBrace brace = system->OpenVideoFile (_fm.film_path.string ());

  m_video_texture = brace.video_texture;
  m_bgfx_state = system->GetVideoBGFXState() | BGFX_STATE_PT_TRISTRIP;
}


VideoRenderable::~VideoRenderable ()
{ }


ch_ptr<VideoTexture> const &VideoRenderable::GetVideoTexture () const
{
  return m_video_texture;
}

ch_ptr<VideoPipeline> VideoRenderable::GetVideoPipeline () const
{
  if (auto *system = VideoSystem::GetSystem(); system)
    return system->FindVideoPipeline (GetVideoTexture ());

  return {};
}


void VideoRenderable::Draw (u16 vyu_id)
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture (0)))
    return;

  bgfx::setTransform (&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics
    (m_bgfx_state  |
     BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                            BGFX_STATE_BLEND_INV_SRC_ALPHA),
     false);

  bgfx::setVertexCount(4);

  // v2i32 const dim = m_video_texture->GetDimensions ();
  // glm::vec4 vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  // bgfx::setUniform(m_video_texture->GetDimensionUniform (), glm::value_ptr (vid_dim));

  // glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  // glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  // bgfx::setUniform(m_video_texture->GetOverUniform(), &over);
  // bgfx::setUniform(m_video_texture->GetUpUniform(), &up);

  // bgfx::submit(vyu_id, m_video_texture->GetProgram ());
  v2i32 const dim = m_video_texture->GetDimensions ();

  glm::vec4 const vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  glm::vec4 const matte_dim {1.0f, 1.0f, 1.0f, 1.0f};

  bgfx::setUniform(m_video_texture->GetDimensionUniform(), glm::value_ptr (vid_dim));
  bgfx::setUniform(m_video_texture->GetMatteDimUniform(),  glm::value_ptr (matte_dim));

  glm::vec4 const flags {f32 (1.0), 0.0f, 0.0f, 0.0f};
  bgfx::setUniform(m_video_texture->GetFlagsUniform(), glm::value_ptr (flags));

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  glm::vec4 const ac = as_glm (m_adjc);
  bgfx::setUniform (m_video_texture->GetOverUniform(), &over);
  bgfx::setUniform (m_video_texture->GetUpUniform(), &up);
  bgfx::setUniform (m_video_texture->GetAdjColorUniform(), glm::value_ptr (ac));

  bgfx::submit(vyu_id, m_video_texture->GetProgram());

}


}
