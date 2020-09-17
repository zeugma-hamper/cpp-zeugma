#ifndef RECTANGLE_RENDERABLE
#define RECTANGLE_RENDERABLE

#include <Renderable.hpp>

#include <bgfx_utils.hpp>

namespace charm
{

class RectangleRenderable final : public Renderable
{
 public:
  RectangleRenderable ();
  ~RectangleRenderable () override;

  void Draw () override;

 protected:
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;
};

}

#endif //RECTANGLE_RENDERABLE
