#include <OverlayInstancing.hpp>

#include <Node.hpp>

#include <vector_interop.hpp>

namespace charm
{

OverlayInstancing::OverlayInstancing ()
  : OverlayInstancing {nullptr}
{
}

OverlayInstancing::OverlayInstancing (Node *_node)
  : Renderable (_node),
    m_flags {1.0f, 1.0f, 0.0f, 0.0f}
{
  m_uni_location = bgfx::createUniform("u_location", bgfx::UniformType::Vec4);
  m_uni_flags = bgfx::createUniform("u_flags", bgfx::UniformType::Vec4);
  m_uni_adj_color = bgfx::createUniform("u_adj_color", bgfx::UniformType::Vec4);
  m_program = CreateProgram("overlay_instance_vs.bin", "overlay_instance_fs.bin", true).program;
}

OverlayInstancing::~OverlayInstancing ()
{
  bgfx::destroy(m_program);
  bgfx::destroy(m_uni_adj_color);
}

void OverlayInstancing::SetRadius (f32 _rad)
{
  m_flags.x = _rad;
}

void OverlayInstancing::SetPoints (Vect *_points, u32 _count)
{
  m_points.clear ();
  m_points.reserve (_count);
  for (u32 i = 0; i < _count; ++i)
    m_points.push_back(glm::vec4 (as_glm(_points[i]), 0.0));
}

void OverlayInstancing::SetPoints (std::vector<Vect> const &_points)
{
  m_points.clear ();
  m_points.reserve (_points.size ());
  for (Vect const &pt : _points)
    m_points.push_back(glm::vec4 (as_glm(pt), 0.0));
}

void OverlayInstancing::Draw (u16 _view_id)
{
  if (m_points.size () == 0)
    return;

  u64 const state =
    BGFX_STATE_PT_TRISTRIP |
    BGFX_STATE_WRITE_RGB |
    BGFX_STATE_WRITE_A   |
    BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                          BGFX_STATE_BLEND_INV_SRC_ALPHA);

  for (glm::vec4 const &pt : m_points)
    {
      bgfx::setState (state);
      bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

      bgfx::setVertexCount(4);

      glm::vec4 const ac = as_glm (m_adjc.val);

      bgfx::setUniform(m_uni_location, glm::value_ptr (pt));
      bgfx::setUniform(m_uni_adj_color, glm::value_ptr (ac));
      bgfx::setUniform(m_uni_flags, glm::value_ptr (m_flags));
      bgfx::submit (_view_id, m_program);
    }
}

}
