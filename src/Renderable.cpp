
#include "Renderable.hpp"


namespace zeugma  {


Renderable::Renderable ()
  : Renderable {nullptr}
{ }

Renderable::Renderable (Node *_node)
  : m_node {_node},
    m_over {Vect::xaxis},
    m_up {Vect::yaxis},
    m_sort_key {0u},
    m_graph_id {0u},
    m_should_draw {true}
{ }

Renderable::~Renderable ()
{ }


const Vect &Renderable::Over ()  const
{ return m_over.val; }

const Vect &Renderable::Up ()  const
{ return m_up.val; }

const ZeColor &Renderable::AdjColor ()  const
{ return m_adjc.val; }


void Renderable::SetOver (const Vect &o)
{ m_over = o; }

void Renderable::SetUp (const Vect &u)
{ m_up = u; }

void Renderable::SetAdjColor (const ZeColor &c)
{ m_adjc = c; }

void Renderable::SetAdjColor (const ZoftColor &zc)
{ m_adjc . BecomeLike (zc); }


ZoftVect &Renderable::OverZoft ()
{ return m_over; }

ZoftVect &Renderable::UpZoft ()
{ return m_up; }

ZoftColor &Renderable::AdjColorZoft ()
{ return m_adjc; }


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

void Renderable::SetGraphID (graph_id _id)
{
  m_graph_id = _id;
}


}
