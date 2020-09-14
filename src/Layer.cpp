#include "Layer.hpp"

#include <algorithm>

namespace charm {

Layer::Layer ()
  : m_root_node {},
    m_projection_matrix {1.0f},
    m_camera_matrix {1.0f}
{
  m_root_node.SetLayer(this);
}

Layer::~Layer ()
{
  m_renderables.clear ();
}

Node *Layer::GetRootNode ()
{
  return &m_root_node;
}

std::vector<Renderable *> &Layer::GetRenderables ()
{
  return m_renderables;
}

glm::mat4 const &Layer::GetProjectionMatrix () const
{
  return m_projection_matrix;
}

void Layer::SetProjectionMatrix (glm::mat4 const &_proj)
{
  m_projection_matrix = _proj;
}

glm::mat4 const &Layer::GetCameraMatrix () const
{
  return m_camera_matrix;
}

void Layer::SetCameraMatrix (glm::mat4 const &_cam)
{
  m_camera_matrix = _cam;
}

void Layer::RemoveRenderable (Renderable *_rend)
{
  m_renderables.erase (std::remove (m_renderables.begin (), m_renderables.end (), _rend));
}

void Layer::RemoveRenderables (std::vector<Renderable *> const &_rends)
{
  auto pred = [&_rends] (Renderable *_r) -> bool
  {
    return std::find (_rends.begin (), _rends.end (), _r) != _rends.end ();
  };

  m_renderables.erase (std::remove_if (m_renderables.begin (), m_renderables.end (), pred),
                       m_renderables.end ());
}
}
