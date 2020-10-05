#include <RectangleRenderable.hpp>

#include <Node.hpp>

#include <charm_glm.hpp>

namespace charm
{

RectangleRenderable::RectangleRenderable ()
    : Renderable (),
      program {BGFX_INVALID_HANDLE},
      vbh {BGFX_INVALID_HANDLE}
  {
    struct vvertex
    {
      glm::vec3 position;
      glm::vec4 color;
    };

    const vvertex combined[] = {
      {{-0.5f,  0.5, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
      {{-0.5f, -0.5, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{ 0.5f,  0.5, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{ 0.5f, -0.5, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
    };

    bgfx::VertexLayout layout;
    layout.begin ()
      .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add (bgfx::Attrib::Color0,   4, bgfx::AttribType::Float)
      .end ();

    const bgfx::Memory *vb_mem = bgfx::copy (combined, 4 * sizeof (vvertex));
    vbh = bgfx::createVertexBuffer(vb_mem, layout);

    // see note about building shaders above
    bx::FilePath shader_path = "quad_vs.bin";
    bgfx::ShaderHandle vs = CreateShader (shader_path);
    shader_path = bx::StringView("quad_fs.bin");
    bgfx::ShaderHandle fs = CreateShader (shader_path);

    if (bgfx::isValid(vs) && bgfx::isValid (fs))
      {
        program = bgfx::createProgram(vs, fs, true);
      }
    else
      {
        bgfx::destroy (vs);
        bgfx::destroy (fs);
      }
  }

  RectangleRenderable::~RectangleRenderable ()
  {
    bgfx::destroy(vbh);
    bgfx::destroy(program);
  }

  void RectangleRenderable::Draw (u16 vyu_id)
  {
    bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);
    bgfx::setVertexBuffer(0, vbh, 0, 4);
    bgfx::setState (BGFX_STATE_WRITE_RGB |
                    BGFX_STATE_PT_TRISTRIP |
                    BGFX_STATE_WRITE_Z);
    bgfx::submit(vyu_id, program, m_graph_id);
  }

}
