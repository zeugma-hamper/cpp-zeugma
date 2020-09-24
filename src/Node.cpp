#include "Node.hpp"

#include <Layer.hpp>
#include <Renderable.hpp>

#include <charm_glm.hpp>

#include <algorithm>

namespace charm {

Node::Node ()
  : m_layer {nullptr},
    m_parent {nullptr}
{
}

Node::~Node ()
{
  for (Node *node : m_children)
    delete node;
  m_children.clear ();

  for (Renderable *rend : m_renderables)
    delete rend;
  m_renderables.clear ();

  m_parent = nullptr;
  m_layer = nullptr;
}

void Node::UpdateTransformsHierarchically ()
{
  bool needs_update = m_local_tx_dirty_flag;
  if (needs_update)
    {
      m_absolute_tx = m_local_tx;
      m_local_tx_dirty_flag = false;
    }

  UpdateTransformsHierarchically(m_absolute_tx, needs_update);
}

void Node::UpdateTransformsHierarchically (Transformation const &_parent, bool _is_dirty)
{
  bool needs_update = _is_dirty || m_local_tx_dirty_flag;
  if (needs_update)
    {
      m_absolute_tx.model = _parent.model * m_local_tx.model;
      m_absolute_tx.normal = _parent.normal * m_local_tx.normal;
      m_local_tx_dirty_flag = false;
    }

  size_t const child_count = m_children.size ();
  for (size_t i = 0u; i < child_count; ++i)
    m_children[i]->UpdateTransformsHierarchically (m_absolute_tx, needs_update);
}

Transformation const &Node::GetAbsoluteTransformation () const
{
  return m_absolute_tx;
}

struct RenderableEnumerator
{
  void operator () (Node &_node)
  {
    std::vector<Renderable *> &rs = _node.GetRenderables();
    size_t const rend_count = rs.size ();
    for (size_t i = 0; i < rend_count; ++i)
      rs[i]->SetGraphID (id++);
  }

  graph_id id = 0u;
};

void Node::EnumerateRenderables()
{
  RenderableEnumerator re;
  VisitDepthFirst (re);
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
    m_layer->RemoveRenderables(m_renderables);

  m_layer = _layer;
  if (m_layer)
    m_layer->m_renderables.insert(m_layer->m_renderables.end (),
                                  m_renderables.begin (),
                                  m_renderables.end ());

  for (Node *child : m_children)
    child->SetLayer (_layer);
}

Layer *Node::GetLayer () const
{
  return m_layer;
}

}
