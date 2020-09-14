#include "Node.hpp"

#include <Layer.hpp>
#include <Renderable.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

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

void Node::UpdateTransformations ()
{
  TransformationSoftValue tsv;
  UpdateTransformations (tsv);
}

void Node::UpdateTransformations (TransformationSoftValue const &_parent_tx)
{
  m_absolute_tx.ClearDirty ();

  if (m_tx_components.IsDirty ())
    {
      //the inconsistency here drives me bonkers
      m_tx_components.ClearDirty ();
      m_tx.SetDirty (true);
      TransformComponents &txc = m_tx_components.GetValue ();
      Transformation &tx = m_tx.GetValue ();
      glm::mat4 const rs = glm::mat4_cast(txc.rotation) * glm::scale (txc.scale);

      tx.model = glm::translate (txc.translation) * rs;
      tx.normal = glm::inverseTranspose (rs);
    }

  if (m_tx.IsDirty() || _parent_tx.IsDirty ())
    {
      Transformation &abs_tx = m_absolute_tx.GetValue ();
      abs_tx.model = _parent_tx.GetValue ().model * m_tx.GetValue ().model;
      abs_tx.normal = _parent_tx.GetValue ().normal * m_tx.GetValue ().normal;
      m_absolute_tx.SetDirty(true);
      m_tx.ClearDirty ();
    }

  size_t const child_count = m_children.size ();
  for (size_t i = 0u; i < child_count; ++i)
    m_children[i]->UpdateTransformations (m_absolute_tx);
}

void Node::EnumerateRenderables()
{
  graph_id id = 0u;
  EnumerateRenderables(id);
}

void Node::EnumerateRenderables (graph_id &_id)
{
  size_t const rend_count = m_renderables.size ();
  for (size_t i = 0; i < rend_count; ++i)
    m_renderables[i]->m_graph_id = _id++;

  size_t const child_count = m_children.size ();
  for (size_t i = 0; i < child_count; ++i)
    m_children[i]->EnumerateRenderables (_id);
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

TransformComponentsSoftValue &Node::GetTransformComponentsSoft ()
{
  return m_tx_components;
}

glm::vec3 const &Node::GetTranslation () const
{
  return m_tx_components.GetTranslation();
}

glm::quat const &Node::GetRotation () const
{
  return m_tx_components.GetRotation();
}

glm::vec3 const &Node::GetScale () const
{
  return m_tx_components.GetScale();
}

TransformationSoftValue &Node::GetAbsoluteTransformationSoft ()
{
  return m_absolute_tx;
}

glm::mat4 const &Node::GetAbsoluteModelTransformation () const
{
  return m_absolute_tx.GetModel();
}

glm::mat4 const &Node::GetAbsoluteNormalTransformation () const
{
  return m_absolute_tx.GetNormal();
}

TransformationAnimSoftValue &Node::GetTransformationSoft ()
{
  return m_tx;
}

glm::mat4 const &Node::GetModelTransformation () const
{
  return m_tx.GetModel();
}

glm::mat4 const &Node::GetNormalTransformation () const
{
  return m_tx.GetNormal();
}

void Node::InstallComponentAnimation (ComponentAnimation *_animation)
{
  if (_animation)
    {
      _animation->SetSoftValue(&m_tx_components);
      m_tx_components.SetAnimation(_animation);
      m_tx_components.SetDirty(true);
      AnimationSystem::GetSystem()->AddAnimation(_animation);
    }
}

Animation *Node::GetComponentAnimation () const
{
  return m_tx_components.GetAnimation();
}


void Node::InstallTransformAnimation (TransformationAnimation *_animation)
{
  if (_animation)
    {
      _animation->SetSoftValue(&m_tx);
      m_tx.SetAnimation(_animation);
      m_tx.SetDirty(true);
      AnimationSystem::GetSystem()->AddAnimation(_animation);
    }
}

Animation *Node::GetTransformationAnimation () const
{
  return m_tx.GetAnimation();
}

}
