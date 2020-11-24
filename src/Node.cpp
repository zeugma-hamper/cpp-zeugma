#include "Node.hpp"

#include <Layer.hpp>
#include <Renderable.hpp>
#include <Frontier.hpp>

#include "vector_interop.hpp"

#include "RoGrappler.h"
#include "ScGrappler.h"
#include "TrGrappler.h"

#include <charm_glm.hpp>

#include <algorithm>

namespace charm {

Node::Node ()
  : m_layer {nullptr},
    m_parent {nullptr},
    m_graps {nullptr},
    m_frontier {nullptr},
    m_id {0u},
    m_local_tx_dirty_flag {true}
{
}

Node::Node (Renderable *_renderable)
  : Node {}
{
  AppendRenderable (_renderable);
}

Node::~Node ()
{
  for (Node *node : m_children)
    delete node;
  m_children.clear ();

  for (Renderable *rend : m_renderables)
    delete rend;
  m_renderables.clear ();

  delete m_frontier;
  delete m_graps;

  m_parent = nullptr;
  m_layer = nullptr;
}

void Node::UpdateTransformsHierarchically (i64 ratch, f64 thyme)
{
  bool needs_update = m_local_tx_dirty_flag;
  if (needs_update)
    {
      m_absolute_tx = m_local_tx;
      m_local_tx_dirty_flag = false;
    }

  UpdateTransformsHierarchically(m_absolute_tx, needs_update, ratch, thyme);
}

void Node::UpdateTransformsHierarchically (Transformation const &_parent,
                                           bool _is_dirty, i64 ratch, f64 thyme)
{
  if (GrapplerPile *gpile = UnsecuredGrapplerPile ())
    { gpile -> Inhale (ratch, thyme);
      SetLocalTransformation (as_glm (gpile -> PntMat ()),
                              as_glm (gpile -> NrmMat ()));
    }

  bool needs_update = _is_dirty || m_local_tx_dirty_flag;
  if (needs_update)
    {
      m_absolute_tx.model = _parent.model * m_local_tx.model;
      m_absolute_tx.normal = _parent.normal * m_local_tx.normal;
      m_local_tx_dirty_flag = false;
    }

  size_t const child_count = m_children.size ();
  for (size_t i = 0u; i < child_count; ++i)
    m_children[i]->UpdateTransformsHierarchically (m_absolute_tx, needs_update,
                                                   ratch, thyme);
}

Transformation const &Node::GetAbsoluteTransformation () const
{
  return m_absolute_tx;
}

struct RenderableEnumerator
{
  RenderableEnumerator (graph_id _id, graph_id _rend_id)
    : id {_id}, rend_id {_rend_id}
  { }

  void operator () (Node &_node)
  {
    _node.SetGraphID(id++);
    std::vector<Renderable *> &rs = _node.GetRenderables();
    size_t const rend_count = rs.size ();
    for (size_t i = 0; i < rend_count; ++i)
      rs[i]->SetGraphID (rend_id++);
  }

  graph_id id;
  graph_id rend_id;
};

std::array<graph_id, 2> Node::EnumerateGraph (graph_id _base_id, graph_id _base_rend_id)
{
  RenderableEnumerator re {_base_id, _base_rend_id};
  VisitDepthFirst (re);


  return {re.id, re.rend_id};
}

void Node::SetGraphID (graph_id _id)
{
  m_id = _id;
}

graph_id Node::GetGraphID () const
{
  return m_id;
}

void Node::SetLocalTransformation (Transformation const &_local)
{
  m_local_tx = _local;
  m_local_tx_dirty_flag = true;
}

void Node::SetLocalTransformation (glm::mat4 const &_vertex_tx)
{
  m_local_tx.model = _vertex_tx;
  m_local_tx.normal = glm::inverseTranspose(glm::mat3 (_vertex_tx));
  m_local_tx_dirty_flag = true;
}

void Node::SetLocalTransformation (glm::mat4 const &_vertex_tx,
                                   glm::mat4 const &_normal_tx)
{
  m_local_tx.model = _vertex_tx;
  m_local_tx.normal = _normal_tx;
  m_local_tx_dirty_flag = true;
}


GrapplerPile *Node::AssuredGrapplerPile ()
{ if (! m_graps)
    m_graps = new GrapplerPile;
  return m_graps;
}

GrapplerPile *Node::UnsecuredGrapplerPile ()
{ return m_graps; }


Grappler *Node::FindGrappler (const std::string &nm)
{ GrapplerPile *gp = UnsecuredGrapplerPile ();
  return gp  ?  gp -> FindGrappler (nm) :  NULL;
}


Grappler *Node::Translate (const Vect &tr)
{ Grappler *g = new TrGrappler (tr);
  AssuredGrapplerPile () -> AppendGrappler (g);
  return g;
}

Grappler *Node::Scale (const Vect &sc)
{ Grappler *g = new ScGrappler (sc);
  AssuredGrapplerPile () -> AppendGrappler (g);
  return g;
}

Grappler *Node::Rotate (const Vect &ax, f64 an)
{ Grappler *g = new RoGrappler (ax, an);
  AssuredGrapplerPile () -> AppendGrappler (g);
  return g;
}

Grappler *Node::RotateWithCenter (const Vect &ax, f64 an, const Vect &ce)
{ Grappler *g = new RoGrappler (ax, an, ce);
  AssuredGrapplerPile () -> AppendGrappler (g);
  return g;
}


Grappler *Node::Translate (ZoftVect &tzo)
{ TrGrappler *tgr = new TrGrappler;
  tgr -> TranslationZoft () . BecomeLike (tzo);
  AssuredGrapplerPile () -> AppendGrappler (tgr);
  return tgr;
}

Grappler *Node::Scale (ZoftVect &szo)
{ ScGrappler *sgr = new ScGrappler;
  sgr -> ScaleZoft () . BecomeLike (szo);
  AssuredGrapplerPile () -> AppendGrappler (sgr);
  return sgr;
}

Grappler *Node::Rotate (ZoftVect &axzo, ZoftFloat &anzo,
                        ZoftVect &cezo, ZoftFloat &phzo)
{ RoGrappler *rgr = new RoGrappler (Vect::zaxis);
  rgr -> AxisZoft () . BecomeLike (axzo);
  rgr -> AngleZoft () . BecomeLike (anzo);
  rgr -> CenterZoft () . BecomeLike (cezo);
  rgr -> PhaseZoft () . BecomeLike (phzo);
  AssuredGrapplerPile () -> AppendGrappler (rgr);
  return rgr;
}


Grappler *Node::Translate (const ZoftVect &tzo)
{ TrGrappler *tgr = new TrGrappler;
  tgr -> TranslationZoft () . BecomeLike (tzo);
  AssuredGrapplerPile () -> AppendGrappler (tgr);
  return tgr;
}

Grappler *Node::Scale (const ZoftVect &szo)
{ ScGrappler *sgr = new ScGrappler;
  sgr -> ScaleZoft () . BecomeLike (szo);
  AssuredGrapplerPile () -> AppendGrappler (sgr);
  return sgr;
}

Grappler *Node::Rotate (const ZoftVect &axzo, const ZoftFloat &anzo,
                        const ZoftVect &cezo, const ZoftFloat &phzo)
{ RoGrappler *rgr = new RoGrappler (Vect::zaxis);
  rgr -> AxisZoft () . BecomeLike (axzo);
  rgr -> AngleZoft () . BecomeLike (anzo);
  rgr -> CenterZoft () . BecomeLike (cezo);
  rgr -> PhaseZoft () . BecomeLike (phzo);
  AssuredGrapplerPile () -> AppendGrappler (rgr);
  return rgr;
}


void Node::ClearTransforms ()
{ if (m_graps)
    m_graps -> RemoveAllGrapplers ();
}



void Node::SetVisibilityForAllLocalRenderables (bool vis)
{ for (Renderable *re  :  m_renderables)
    re -> SetShouldDraw (vis);
}


Node *Node::Parent ()
{ return m_parent; }

//node takes ownership of child nodes
void Node::AppendChild (Node *_node)
{
  if (_node->m_parent)
    _node->m_parent->ExciseChild (_node);

  _node->m_parent = this;
  _node->SetLayer (m_layer);

  m_children.push_back (_node);
}

void Node::RemoveChild (Node *_node)
{
  if (ExciseChild (_node))
    delete _node;
}

Node *Node::ExciseChild (Node *_node)
{
  auto it = std::find (m_children.begin (), m_children.end (), _node);
  if (it == m_children.end ())
    return nullptr;

  Node *n = *it;
  m_children.erase (it);
  n->SetLayer (nullptr);
  return n;
}

void Node::AppendRenderable (Renderable *_render)
{
  if (! _render)
    return;

  m_renderables.push_back (_render);
  _render->m_node = this;

  if (m_layer)
    m_layer->GetRenderables().push_back(_render);
}

void Node::RemoveRenderable (Renderable *_render)
{
  if (ExciseRenderable(_render))
    delete _render;
}

Renderable *Node::ExciseRenderable (Renderable *_render)
{
  auto it = std::find (m_renderables.begin (), m_renderables.end (), _render);
  if (it == m_renderables.end ())
    return nullptr;

  Renderable *r = *it;
  m_renderables.erase (it);
  if (m_layer)
    m_layer->RemoveRenderable (r);

  return r;
}

std::vector<Renderable *> &Node::GetRenderables ()
{
  return m_renderables;
}

std::vector<Renderable *> const &Node::GetRenderables () const
{
  return m_renderables;
}

void Node::SetLayer (Layer *_layer)
{
  if (m_layer)
    {
      m_layer->RemoveRenderables(m_renderables);
      m_layer->RemoveFrontier(m_frontier);
    }

  m_layer = _layer;
  if (m_layer)
    {
      m_layer->m_renderables.insert(m_layer->m_renderables.end (),
                                    m_renderables.begin (),
                                    m_renderables.end ());
      if (m_frontier)
        m_layer->m_frontiers.push_back(m_frontier);
    }

  for (Node *child : m_children)
    child->SetLayer (_layer);
}

Layer *Node::GetLayer () const
{
  return m_layer;
}

void Node::SetFrontier (Frontier *_frontier)
{
  if (! m_layer)
    {
      delete m_frontier;
      m_frontier = _frontier;
      m_frontier->m_node = this;
      return;
    }

  if (m_frontier)
    {
      m_layer->RemoveFrontier(m_frontier);
      delete m_frontier;
      m_frontier = nullptr;
    }

  if (_frontier)
    {
      _frontier->m_node = this;
      m_frontier = _frontier;
      m_layer->m_frontiers.push_back(_frontier);
    }
}

Frontier *Node::GetFrontier () const
{
  return m_frontier;
}


}
