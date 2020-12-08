#include <OverlayTest.hpp>

#include <Node.hpp>

#include <vector_interop.hpp>

namespace charm
{

OverlayTest::OverlayTest ()
  : OverlayTest {nullptr}
{
}

OverlayTest::OverlayTest (Node *_node)
  : Renderable (_node),
    m_dimensions {0.0f},
    m_flags {0.0f, 0.1f, 0.0f, 0.0f}
{
  m_uni_dimensions = bgfx::createUniform("u_dimensions", bgfx::UniformType::Vec4);
  m_uni_flags = bgfx::createUniform("u_flags", bgfx::UniformType::Vec4);
  m_uni_rois  = bgfx::createUniform("u_rois", bgfx::UniformType::Vec4, 10);
  m_uni_adj_color = bgfx::createUniform("u_adj_color", bgfx::UniformType::Vec4);
  m_program = CreateProgram("overlay_vs.bin", "overlay_fs.bin", true).program;
}

OverlayTest::~OverlayTest ()
{
  bgfx::destroy(m_program);
  bgfx::destroy(m_uni_adj_color);
}

void OverlayTest::SetDimensions (f32 _width, f32 _height)
{
  m_dimensions = glm::vec4 (_width, _height, 0.0f, 0.0f);
}

void OverlayTest::SetRadius (f32 _rad)
{
  m_flags.y = _rad;
}

void OverlayTest::SetPoints (v4f32 *_points, u32 _count)
{
  m_points.clear ();
  m_points.insert (m_points.begin (), _points, _points + _count);
  m_flags.x = f32 (_count);
}

void OverlayTest::SetPoints (std::vector<v4f32> const &_points)
{
  m_points = _points;
  m_flags.x = f32 (m_points.size());
}

void OverlayTest::Draw (u16 _view_id)
{
  if (m_points.size () == 0 ||
      m_dimensions.x == 0.0f ||
      m_dimensions.y == 0.0f)
    return;

  u64 const state =
    BGFX_STATE_PT_TRISTRIP |
    BGFX_STATE_WRITE_RGB |
    BGFX_STATE_WRITE_A   |
    BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA,
                          BGFX_STATE_BLEND_INV_SRC_ALPHA);
  bgfx::setState (state);

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);
  bgfx::setVertexCount(4);

  glm::vec4 const ac = as_glm (m_adjc.val);

  bgfx::setUniform(m_uni_adj_color, glm::value_ptr (ac));
  bgfx::setUniform(m_uni_flags, glm::value_ptr (m_flags));
  bgfx::setUniform(m_uni_dimensions, glm::value_ptr (m_dimensions));
  u32 const count = std::min (10u, u32 (m_points.size ()));
  bgfx::setUniform(m_uni_rois, m_points.data (), count);

  bgfx::submit (_view_id, m_program);
}

}
