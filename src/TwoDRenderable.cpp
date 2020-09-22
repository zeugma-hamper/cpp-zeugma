#include <TwoDRenderable.hpp>

namespace charm
{

TwoDRenderable::TwoDRenderable ()
  : TwoDRenderable {nullptr}
{ }

TwoDRenderable::TwoDRenderable (Node *_node)
  : Renderable {_node},
    m_width {1.0f},
    m_height {1.0f}
{ }

void TwoDRenderable::SetWidth (f32 _width)
{
  m_width = _width;
}

f32 TwoDRenderable::GetWidth () const
{
  return m_width;
}

void TwoDRenderable::SetHeight (f32 _height)
{
  m_height = _height;
}

f32 TwoDRenderable::GetHeight () const
{
  return m_height;
}


}
