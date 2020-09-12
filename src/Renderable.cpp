#include "Renderable.hpp"

namespace charm {

Renderable::Renderable ()
  : Renderable {nullptr}
{ }

Renderable::Renderable (Node *_node)
  : m_node {_node},
    m_sort_key {0u},
    m_graph_id {0u},
    m_should_draw {true}
{}

Renderable::~Renderable ()
{ }

void Renderable::update ()
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

sort_key Renderable::get_sort_key () const
{
  return m_sort_key;
}

graph_id Renderable::get_graph_id () const
{
  return m_graph_id;
}

}
