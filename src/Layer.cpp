#include "Layer.hpp"

#include <algorithm>

namespace charm {

Layer::Layer ()
  : m_root_node {},
    m_projection_matrix {1.0f},
    m_camera_matrix {1.0f}
{
  m_root_node.set_layer(this);
}

Layer::~Layer ()
{
  m_renderables.clear ();
}

Node *Layer::root_node ()
{
  return &m_root_node;
}

std::vector<Renderable *> &Layer::get_renderables ()
{
  return m_renderables;
}

glm::mat4 const &Layer::get_projection_matrix () const
{
  return m_projection_matrix;
}

void Layer::set_projection_matrix (glm::mat4 const &_proj)
{
  m_projection_matrix = _proj;
}

glm::mat4 const &Layer::get_camera_matrix () const
{
  return m_camera_matrix;
}

void Layer::set_camera_matrix (glm::mat4 const &_cam)
{
  m_camera_matrix = _cam;
}

void Layer::remove_renderable (Renderable *_rend)
{
  m_renderables.erase (std::remove (m_renderables.begin (), m_renderables.end (), _rend));
}

void Layer::remove_renderables (std::vector<Renderable *> const &_rends)
{
  auto pred = [&_rends] (Renderable *_r) -> bool
  {
    return std::find (_rends.begin (), _rends.end (), _r) != _rends.end ();
  };

  m_renderables.erase (std::remove_if (m_renderables.begin (), m_renderables.end (), pred),
                       m_renderables.end ());
}
}
