#include "Renderable.hpp"

namespace charm {

Renderable::Renderable ()
  : Renderable {nullptr}
{ }

Renderable::Renderable (Node *_node)
  : m_node {_node},
    m_over {1.0f, 0.0f, 0.0f, 0.0f},
    m_up {0.0f, 1.0f, 0.0f, 0.0f},
    m_sort_key {0u},
    m_graph_id {0u},
    m_should_draw {true}
{}

Renderable::~Renderable ()
{ }

void Renderable::SetOver (glm::vec4 const &_over)
{
  m_over = _over;
}

glm::vec4 const &Renderable::GetOver () const
{
  return m_over;
}

void Renderable::SetUp (glm::vec4 const &_up)
{
  m_up = _up;
}

glm::vec4 const &Renderable::GetUp () const
{
  return m_up;
}

void Renderable::Update ()
{
}

bool Renderable::ShouldDraw () const
{
  return m_should_draw;
}

void Renderable::SetShouldDraw (bool _tf)
{
  m_should_draw = _tf;
}

sort_key Renderable::GetSortKey () const
{
  return m_sort_key;
}

graph_id Renderable::GetGraphId () const
{
  return m_graph_id;
}

}
