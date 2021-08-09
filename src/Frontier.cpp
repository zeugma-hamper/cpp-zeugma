
//
// (c) treadle & loam, provisioners llc
//

#include <Frontier.hpp>

#include <Node.hpp>
#include <Renderable.hpp>

#include <vector_interop.hpp>


namespace zeugma  {


Frontier::Frontier ()
  : Frontier {nullptr}
{ }

Frontier::Frontier (Node *_node)
  : m_node {_node}
{ }

Frontier::~Frontier ()
{
  m_node = nullptr;
}

Node *Frontier::ItsNode () const
{
  return m_node;
}

AABB Frontier::GetLocalAABB () const
{
  return AABB{};
}

AABB Frontier::GetGlobalAABB () const
{
  return AABB {};
}

RectangleFrontier::RectangleFrontier (Vect const &_bl, Vect const &_tr, Vect const &_norm)
  : m_bl {_bl}, m_tr {_tr}, m_norm {_norm}
{  }

AABB RectangleFrontier::GetLocalAABB  () const
{
  return AABB {m_bl, m_tr};
}

static Vect compwise_min (Vect const &_l, Vect const &_r)
{
  return Vect {std::min (_l.x, _r.x),
               std::min (_l.y, _r.y),
               std::min (_l.z, _r.z)};
}

static Vect compwise_max (Vect const &_l, Vect const &_r)
{
  return Vect {std::max (_l.x, _r.x),
               std::max (_l.y, _r.y),
               std::max (_l.z, _r.z)};
}

AABB RectangleFrontier::GetGlobalAABB () const
{
  if (! m_node || ! m_node->UnsecuredGrapplerPile())
    return AABB {m_bl, m_tr};

  GrapplerPile *const pl = m_node->UnsecuredGrapplerPile();

  Vect const t_bl = pl->pnt_mat.TransformVect(m_bl);
  Vect const t_tr = pl->pnt_mat.TransformVect(m_tr);

  return AABB {compwise_min(t_bl, t_tr), compwise_max(t_bl, t_tr)};
}

bool RectangleFrontier::CheckHit (G::Ray const &_ray, Vect *_hit_pt) const
{ if (! m_node)
    return false;

  GrapplerPile *const pl = m_node->UnsecuredGrapplerPile();
  Vect const t_bl = pl ? pl->pnt_mat.TransformVect(m_bl) : m_bl;
  Vect const t_tr = pl ? pl->pnt_mat.TransformVect(m_tr) : m_tr;
  Vect const t_norm = pl ? pl->nrm_mat.TransformVect(m_norm) : m_norm;

  Vect const diag = t_tr - t_bl;
  Vect const diag_norm = (t_tr - t_bl).Norm ();
  Vect const tmp = t_norm.Cross(diag_norm);
  Vect const over = 0.5 * (diag_norm + tmp);
  Vect const up = 0.5 * (diag_norm - tmp);

  return G::RayRectIntersection (_ray.orig, _ray.dir,
                                 0.5 * (t_tr + t_bl), over, up,
                                 diag.Dot (over), diag.Dot (up), _hit_pt);
}

bool RectangleFrontier::CheckPlaneHit (G::Ray const &_ray,
                                       Vect *_hit_pt)  const
{ if (! m_node)
    return false;

  GrapplerPile *const pl = m_node->UnsecuredGrapplerPile();
  Vect const t_bl = pl  ?  pl->pnt_mat . TransformVect (m_bl)  :  m_bl;
  Vect const t_tr = pl  ?  pl->pnt_mat . TransformVect (m_tr)  :  m_tr;
  Vect const t_norm = pl  ?  pl->nrm_mat . TransformVect (m_norm)  :  m_norm;

  return G::RayPlaneIntersection (_ray.orig, _ray.dir,
                                  0.5 * (t_tr + t_bl), t_norm, _hit_pt);
}


RectRenderableFrontier::RectRenderableFrontier (Renderable *_renderable,
                                                Vect const &_bl, Vect const &_tr)
  : m_renderable {_renderable}, m_bl {_bl}, m_tr {_tr}
{  }

RectRenderableFrontier::RectRenderableFrontier (Renderable *_rend,
                                                Vect const &_p, f64 _w, f64 _h)
 :  m_renderable (_rend), m_pos (_p), m_wid (_w), m_hei (_h)
{ }

Renderable *RectRenderableFrontier::GetRenderable () const
{
  return m_renderable;
}

AABB RectRenderableFrontier::GetLocalAABB  () const
{
  Vect const over = m_renderable ? m_renderable->Over () : Vect::xaxis;
  Vect const up = m_renderable ? m_renderable->Up () : Vect::yaxis;
  Vect const norm = over.Cross(up);
  Vect const bl = Vect (m_bl.Dot (over), m_bl.Dot (up), m_bl.Dot (norm));
  Vect const tr = Vect (m_tr.Dot (over), m_tr.Dot (up), m_tr.Dot (norm));
  return AABB {compwise_min(bl, tr), compwise_max(bl, tr)};
}

AABB RectRenderableFrontier::GetGlobalAABB () const
{
  if (! m_node || ! m_node->UnsecuredGrapplerPile())
    return GetLocalAABB();

  AABB const aabb = GetLocalAABB();

  Matrix44 const m = from_glm (m_node->GetAbsoluteTransformation().model);
  Vect const t_bl = m.TransformVect(aabb.blf);
  Vect const t_tr = m.TransformVect(aabb.trb);

  return AABB {compwise_min(t_bl, t_tr), compwise_max(t_bl, t_tr)};
}

bool RectRenderableFrontier::CheckHit (G::Ray const &_ray, Vect *_hit_pt) const
{
  if (! m_node  ||  ! m_renderable)
    return false;

  // AABB const aabb = GetGlobalAABB();
  // Vect const cent = 0.5 * (aabb.blf + aabb.trb);
  // f64 const width = std::fabs((aabb.trb - aabb.blf).Dot (m_renderable->Over ()));
  // f64 const height = std::fabs((aabb.trb - aabb.blf).Dot (m_renderable->Up ()));
  // return G::RayRectIntersection(_ray.orig, _ray.dir,
  //                               cent, m_renderable->Over (), m_renderable->Up (),
  //                               width, height, _hit_pt);

  Matrix44 const m = from_glm (m_node -> GetAbsoluteTransformation ().model);
  Vect p = m . TransformVect (m_pos);
  Vect z = m . TransformVect (Vect::zerov);
  Vect o = m . TransformVect (m_wid * m_renderable -> Over ())  -  z;
  Vect u = m . TransformVect (m_hei * m_renderable -> Up ())  -  z;
  f64 ww = o . NormSelfReturningMag ();
  f64 hh = u . NormSelfReturningMag ();
  return G::RayRectIntersection (_ray.orig, _ray.dir,
                                 p, o, u, ww, hh, _hit_pt);

  // GrapplerPile *const pl = m_node->UnsecuredGrapplerPile();
  // Vect const t_bl = pl ? pl->pnt_mat.TransformVect(m_bl) : m_bl;
  // Vect const t_tr = pl ? pl->pnt_mat.TransformVect(m_tr) : m_tr;
  // Vect const t_norm = pl ? pl->nrm_mat.TransformVect(m_norm) : m_norm;

  // Vect const diag_norm = (t_tr - t_bl).Norm ();
  // Vect const tmp = t_norm.Cross(diag_norm);
  // Vect const over = 0.5 * (diag_norm + tmp);
  // Vect const up = 0.5 * (diag_norm - tmp);

  // return G::RayRectIntersection(_ray.orig, _ray.dir,
  //                               0.5 * (t_tr + t_bl), over, up,
  //                               diag.Dot (over), diag.Dot (up), _hit_pt);
}

bool RectRenderableFrontier::CheckPlaneHit (G::Ray const &_ray,
                                            Vect *_hit_pt)  const
{
  if (! m_node  ||  ! m_renderable)
    return false;

  Matrix44 const m = from_glm (m_node -> GetAbsoluteTransformation ().model);
  Vect p = m . TransformVect (m_pos);
  Vect z = m . TransformVect (Vect::zerov);
  Vect o = m . TransformVect (m_wid * m_renderable -> Over ())  -  z;
  Vect u = m . TransformVect (m_hei * m_renderable -> Up ())  -  z;
  Vect n = o . Cross (u);
  return G::RayPlaneIntersection (_ray.orig, _ray.dir, p, n, _hit_pt);
}


}
