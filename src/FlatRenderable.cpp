
#include <FlatRenderable.hpp>


namespace zeugma  {


FlatRenderable::FlatRenderable ()
  : FlatRenderable {nullptr}
{ }

FlatRenderable::FlatRenderable (Node *_node)
  : Renderable {_node},
    m_width {1.0f},
    m_height {1.0f}
{ }

void FlatRenderable::SetWidth (f32 _width)
{
  m_width = _width;
}

f32 FlatRenderable::GetWidth () const
{
  return m_width;
}

void FlatRenderable::SetHeight (f32 _height)
{
  m_height = _height;
}

f32 FlatRenderable::GetHeight () const
{
  return m_height;
}


}
