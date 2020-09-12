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

void Node::update_transformations ()
{
  TransformationSoftValue tsv {{glm::mat4{1.0}, glm::mat4{1.0f}}};
  update_transformations (tsv);
}

void Node::update_transformations (TransformationSoftValue const &_parent_tx)
{
  m_absolute_tx.clear_dirty ();

  if (m_tx_components.is_dirty ())
    {
      //the inconsistency here drives me bonkers
      m_tx_components.clear_dirty ();
      m_tx.set_dirty (true);
      TransformComponents &txc = m_tx_components.get_value ();
      Transformation &tx = m_tx.get_value ();
      glm::mat4 const rs = glm::mat4_cast(txc.rotation) *
        glm::scale (txc.scale);

      tx.model = glm::translate (txc.translation) * rs;
      tx.normal = glm::inverseTranspose (rs);
    }

  if (m_tx.is_dirty() || _parent_tx.is_dirty ())
    {
      Transformation &abs_tx = m_absolute_tx.get_value ();
      abs_tx.model = _parent_tx.get_value ().model * m_tx.get_value ().model;
      abs_tx.normal = _parent_tx.get_value ().normal * m_tx.get_value ().normal;
      m_absolute_tx.set_dirty(true);
      m_tx.clear_dirty ();
    }

  size_t const child_count = m_children.size ();
  for (size_t i = 0u; i < child_count; ++i)
    m_children[i]->update_transformations (m_absolute_tx);
}

void Node::enumerate_renderables()
{
  graph_id id = 0u;
  enumerate_renderables(id);
}

void Node::enumerate_renderables (graph_id &_id)
{
  size_t const rend_count = m_renderables.size ();
  for (size_t i = 0; i < rend_count; ++i)
    m_renderables[i]->m_graph_id = _id++;

  size_t const child_count = m_children.size ();
  for (size_t i = 0; i < child_count; ++i)
    m_children[i]->enumerate_renderables (_id);
}

//node takes ownership of child nodes
void Node::append_child (Node *_node)
{
  if (_node->m_parent)
    _node->m_parent->excise_child (_node);

  _node->m_parent = this;
  _node->set_layer (m_layer);

  m_children.push_back (_node);
}

void Node::remove_child (Node *_node)
{
  if (excise_child (_node))
    delete _node;
}

Node *Node::excise_child (Node *_node)
{
  auto it = std::find (m_children.begin (), m_children.end (), _node);
  if (it == m_children.end ())
    return nullptr;

  Node *n = *it;
  m_children.erase (it);
  n->set_layer (nullptr);
  return n;
}

void Node::append_renderable (Renderable *_render)
{
  if (! _render)
    return;

  m_renderables.push_back (_render);
  _render->m_node = this;

  if (m_layer)
    m_layer->get_renderables().push_back(_render);
}

void Node::remove_renderable (Renderable *_render)
{
  if (excise_renderable(_render))
    delete _render;
}

Renderable *Node::excise_renderable (Renderable *_render)
{
  auto it = std::find (m_renderables.begin (), m_renderables.end (), _render);
  if (it == m_renderables.end ())
    return nullptr;

  Renderable *r = *it;
  m_renderables.erase (it);
  if (m_layer)
    m_layer->remove_renderable (r);

  return r;
}

void Node::set_layer (Layer *_layer)
{
  if (m_layer)
    m_layer->remove_renderables(m_renderables);

  m_layer = _layer;
  if (m_layer)
    m_layer->m_renderables.insert(m_layer->m_renderables.end (),
                                  m_renderables.begin (),
                                  m_renderables.end ());

  for (Node *child : m_children)
    child->set_layer (_layer);
}

Layer *Node::get_layer () const
{
  return m_layer;
}

TransformComponentsSoftValue &Node::get_transform_components_soft ()
{
  return m_tx_components;
}


TransformationSoftValue &Node::get_absolute_transformation_soft ()
{
  return m_absolute_tx;
}

glm::mat4 const &Node::get_absolute_model_transformation () const
{
  return m_absolute_tx.get_value().model;
}

glm::mat4 const &Node::get_absolute_normal_transformation () const
{
  return m_absolute_tx.get_value().normal;
}


TransformationSoftValue &Node::get_transformation_soft ()
{
  return m_tx;
}

glm::mat4 const &Node::get_model_transformation () const
{
  return m_tx.get_value().model;
}

glm::mat4 const &Node::get_normal_transformation () const
{
  return m_tx.get_value().normal;
}


}
