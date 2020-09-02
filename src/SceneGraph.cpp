#include "SceneGraph.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>

namespace charm {

Renderable::Renderable ()
  : m_sort_key {0u},
    m_graph_id {0u},
    m_should_draw {true}
{}

Renderable::~Renderable ()
{ }

void Renderable::update (Node *)
{
}

bool Renderable::should_draw () const
{
  return m_should_draw;
}

void Renderable::set_should_draw (bool _tf)
{
  m_should_draw = _tf;
}

Renderable::sort_key Renderable::get_sort_key () const
{
  return m_sort_key;
}

Renderable::graph_id Renderable::get_graph_id () const
{
  return m_graph_id;
}

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
        glm::scale (glm::mat4{1.0f}, txc.scale);
      tx.model = glm::translate (rs, txc.translation);
      tx.normal = glm::inverseTranspose(rs);
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
  Renderable::graph_id id = 0u;
  enumerate_renderables(id);
}

void Node::enumerate_renderables (Renderable::graph_id &_id)
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
  _node->m_layer = m_layer;

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
  return n;
}

void Node::append_renderable (Renderable *_render)
{
  m_renderables.push_back (_render);
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
  return r;
}

Layer::Layer ()
{ }

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
