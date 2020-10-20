#include "Layer.hpp"

#include <algorithm>

#include <Frontier.hpp>
#include <Node.hpp>

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

Frontier *Layer::FirstHitFrontier (Ray const &_ray, Vect *_hit_pt) const
{
  for (Frontier *fr : m_frontiers)
    if (fr->CheckHit(_ray, _hit_pt))
      return fr;

  return nullptr;
}


std::vector<Renderable *> &Layer::GetRenderables ()
{
  return m_renderables;
}

std::vector<Frontier *> &Layer::GetFrontiers ()
{
  return m_frontiers;
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

void Layer::SortFrontiers ()
{
  std::sort (m_frontiers.begin (), m_frontiers.end (),
             [] (Frontier const * const _left, Frontier const * const _right)
             { return (_left -> ItsNode () -> GetGraphID ()
                       >  _right -> ItsNode () -> GetGraphID ());
             });
}

void Layer::RemoveFrontier (Frontier *_frontier)
{
  m_frontiers.erase (std::remove (m_frontiers.begin (), m_frontiers.end (), _frontier));
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
