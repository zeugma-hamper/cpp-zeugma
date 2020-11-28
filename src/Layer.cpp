
#include "Layer.hpp"

#include <algorithm>

#include <Frontier.hpp>
#include <Node.hpp>


namespace charm {


Layer::Layer ()
  : m_root_node {}
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

Frontier *Layer::FirstHitFrontier (G::Ray const &_ray, Vect *_hit_pt) const
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


bool Layer::RenderableToForemost (Renderable *re)
{ if (! re)
    return false;

  auto omega = m_renderables . end ();
  auto it = std::find (m_renderables . begin (), omega, re);
  if (it  ==  omega)
    return false;

  if (it  !=  omega - 1)
    { m_renderables . erase (it);
      m_renderables . push_back (re);
    }
  return true;
}

bool Layer::RenderableToRearmost (Renderable *re)
{ if (! re)
    return false;

  auto omega = m_renderables . end ();
  auto alpha = m_renderables . begin ();
  auto it = std::find (m_renderables . begin (), omega, re);
  if (it  ==  omega)
    return false;

  if (it  !=  alpha)
    { m_renderables . erase (it);
      m_renderables . insert (m_renderables . begin (), re);
    }
  return true;
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
  m_frontiers.erase (std::remove (m_frontiers.begin (), m_frontiers.end (), _frontier),
                     m_frontiers.end ());
}

void Layer::RemoveRenderable (Renderable *_rend)
{
  m_renderables.erase (std::remove (m_renderables.begin (), m_renderables.end (), _rend),
                       m_renderables.end ());
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
