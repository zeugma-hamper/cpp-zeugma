#ifndef YEP_LETS_SEE
#define YEP_LETS_SEE

#include <Renderable.hpp>

#include <bgfx_utils.hpp>

namespace charm
{

class Node;

class OverlayTest : public Renderable
{
 public:
  OverlayTest ();
  OverlayTest (Node *_node);
  ~OverlayTest () override;

  void SetDimensions (f32 _width, f32 _height);

  void SetRadius (f32 _rad);

  // xy - min, zw - max
  void SetPoints (v4f32 *_points, u32 _count);
  void SetPoints (std::vector<v4f32> const &_points);

  void Draw (u16 _view_id) override;

 protected:
  bgfx::UniformHandle m_uni_dimensions;
  bgfx::UniformHandle m_uni_flags;
  bgfx::UniformHandle m_uni_rois;
  bgfx::UniformHandle m_uni_adj_color;
  bgfx::ProgramHandle m_program;

  std::vector<v4f32> m_points;
  glm::vec4 m_dimensions;
  glm::vec4 m_flags;
};

}

#endif //YEP_LETS_SEE
