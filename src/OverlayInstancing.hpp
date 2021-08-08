
//
// (c) treadle & loam, provisioners llc
//

#ifndef YEP_LETS_SEE
#define YEP_LETS_SEE


#include <Renderable.hpp>
#include <Vect.h>

#include <bgfx_utils.hpp>


namespace zeugma  {


class Node;

class OverlayInstancing : public Renderable
{
 public:
  OverlayInstancing ();
  OverlayInstancing (Node *_node);
  ~OverlayInstancing () override;

  void SetRadius (f32 _rad);

  // xy - min, zw - max
  void SetPoints (Vect *_points, u32 _count);
  void SetPoints (std::vector<Vect> const &_points);

  void Draw (u16 _view_id) override;

 protected:
  bgfx::UniformHandle m_uni_location;
  bgfx::UniformHandle m_uni_flags;
  bgfx::UniformHandle m_uni_adj_color;
  bgfx::ProgramHandle m_program;

  std::vector<glm::vec4> m_points;
  glm::vec4 m_flags;
};


}


#endif  //YEP_LETS_SEE
