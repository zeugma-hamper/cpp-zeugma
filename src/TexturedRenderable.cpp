#include <TexturedRenderable.hpp>

#include <Node.hpp>

namespace charm
{

TexturedRenderable::TexturedRenderable ()
  : Renderable (),
    m_program {BGFX_INVALID_HANDLE},
    m_vbuffer {BGFX_INVALID_HANDLE},
    m_uni_over {BGFX_INVALID_HANDLE},
    m_uni_up {BGFX_INVALID_HANDLE},
    m_uni_texture {BGFX_INVALID_HANDLE},
    m_particulars {}
{
  m_uni_over = bgfx::createUniform("u_over", bgfx::UniformType::Vec4);
  m_uni_up = bgfx::createUniform("u_up", bgfx::UniformType::Vec4);
  m_uni_texture = bgfx::createUniform("u_texture", bgfx::UniformType::Sampler);

  ProgramResiduals pr = CreateProgram("textured_renderable.vs.bin", "textured_renderable.fs.bin", true);
  m_program = pr.program;
}

TexturedRenderable::TexturedRenderable (TextureParticulars const &_tp)
  : TexturedRenderable ()
{
  SetTexture (_tp);
}

void TexturedRenderable::SetTexture (TextureParticulars const &_tp)
{
  if (bgfx::isValid (m_particulars.handle))
    bgfx::destroy(m_particulars.handle);

  m_particulars = _tp;

  if (bgfx::isValid (m_particulars.handle) &&
      m_particulars.width > 0 &&
      m_particulars.height > 0)
    AdaptGeometryToTexture();
}

TextureParticulars const &TexturedRenderable::GetTexture () const
{
  return m_particulars;
}

void TexturedRenderable::Draw (u16 _view_id)
{
  (void)_view_id;
  if (! bgfx::isValid (m_particulars.handle))
    return;

  u64 state = BGFX_STATE_WRITE_RGB |
    BGFX_STATE_PT_TRISTRIP;

  // TODO: better alpha existence check
  if (m_particulars.format == bgfx::TextureFormat::RGBA8)
    state |= BGFX_STATE_WRITE_A;

  bgfx::setTransform(glm::value_ptr (m_node->GetAbsoluteTransformation ().model));
  bgfx::setVertexBuffer(0, m_vbuffer, 0, 4);
  bgfx::setState (state);
  bgfx::submit (_view_id, m_program, m_graph_id);
}

void TexturedRenderable::AdaptGeometryToTexture ()
{
  // i don't expect this to happen often
  if (bgfx::isValid(m_vbuffer))
    {
      bgfx::destroy(m_vbuffer);
      m_vbuffer = BGFX_INVALID_HANDLE;
    }

  struct uv_vertex
    {
      glm::vec3 position;
      glm::vec2 uv;
    };

  uv_vertex combined[] = {
    {{-0.5f,  0.5, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5, 0.0f}, {0.0f, 1.0f}},
    {{ 0.5f,  0.5, 0.0f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5, 0.0f}, {1.0f, 1.0f}},
  };

  glm::vec3 const aspect_adj
    = m_particulars.width >= m_particulars.height
    ? glm::vec3 {1.0, f32 (m_particulars.height)/m_particulars.width, 1.0}
    : glm::vec3 {f32 (m_particulars.width)/m_particulars.height, 1.0, 1.0};

  for (uv_vertex &v: combined)
    v.position *= aspect_adj;

  bgfx::VertexLayout layout;
  layout.begin ()
    .add (bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
    .add (bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
    .end ();

  const bgfx::Memory *vb_mem = bgfx::copy (combined, 4 * sizeof (uv_vertex));
  m_vbuffer = bgfx::createVertexBuffer(vb_mem, layout);
}

}
