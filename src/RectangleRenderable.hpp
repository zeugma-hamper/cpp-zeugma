
#ifndef RECTANGLE_RENDERABLE
#define RECTANGLE_RENDERABLE


#include <Renderable.hpp>

#include <bgfx_utils.hpp>


namespace zeugma  {


class RectangleRenderable final : public Renderable
{
 public:
  RectangleRenderable ();
  ~RectangleRenderable () override;

  void Draw (u16 vyu_id) override;

 protected:
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;
};


}


#endif  //RECTANGLE_RENDERABLE
