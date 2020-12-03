
#include "GeomFumble.h"


namespace charm  {


bool G::RayPlaneIntersection (const Vect &frm, const Vect &aim,
                              const Vect &pnt, const Vect &nrm,
                              Vect *hit_pnt)
{ f64 t = aim . Dot (nrm);
  if (t  ==  0.0)
    return false;
  t = (pnt - frm) . Dot (nrm) / t;
  if (t  <  0.0)  // behind us, you see
    return false;
  if (hit_pnt)
    *hit_pnt = frm  +  t * aim;
  return true;
}


bool G::LinePlaneIntersection (const Vect &frm, const Vect &aim,
                               const Vect &pnt, const Vect &nrm,
                               Vect *hit_pnt)
{ f64 t = aim . Dot (nrm);
  if (t  ==  0.0)
    return false;
  t = (pnt - frm) . Dot (nrm) / t;
  if (hit_pnt)
    *hit_pnt = frm  +  t * aim;
  return true;
}


bool G::RayRectIntersection (const Vect &frm, const Vect &aim,
                             const Vect &cnt,
                             const Vect &ovr, const Vect &upp,
                             f64 wid, f64 hei, Vect *hit_pnt)
{ Vect pee, p;
  if (! RayPlaneIntersection (frm, aim, cnt, ovr . Cross (upp), &pee))
    return false;
  p = pee - cnt;
  f64 t = 2.0 * p . Dot (ovr);
  if (t >= -wid  &&  t<= wid)
    {t = 2.0 * p . Dot (upp);
      if (t >= -hei  &&  t <= hei)
        { *hit_pnt = pee;  return true; }
    }
  return false;
}


bool G::RayAnnulusIntersection (const Vect &frm, const Vect &aim,
                                const Vect &cnt, const Vect &e0,
                                const Vect &e1, f64 r1, f64 r2,
                                Vect *hit_pnt,
                                f64 *hit_rad, f64 *hit_phi)
{ Vect hit;
  if (! RayPlaneIntersection (frm, aim, cnt, e0 . Cross (e1), &hit))
    return false;
  hit -= cnt;
  f64 arr_sq = hit . AutoDot ();
  if (arr_sq  <  r1 * r1)
    return false;
  if (r2 > 0.0  &&  arr_sq > r2 * r2)
    return false;
  if (hit_pnt)
    *hit_pnt = hit + cnt;
  if (hit_rad)
    *hit_rad = sqrt (arr_sq);
  if (hit_phi)
    *hit_phi = atan2 (e1 . Dot (hit), e0 . Dot (hit));
  return true;
}


bool G::LineAnnulusIntersection (const Vect &frm, const Vect &aim,
                                 const Vect &cnt, const Vect &e0,
                                 const Vect &e1, f64 r1, f64 r2,
                                 Vect *hit_pnt,
                                 f64 *hit_rad, f64 *hit_phi)
{ Vect hit;
  if (! LinePlaneIntersection (frm, aim, cnt, e0 . Cross (e1), &hit))
    return false;
// ready? here it comes! please to enjoy all the great code duplication:
  hit -= cnt;
  f64 arr_sq = hit . AutoDot ();
  if (arr_sq  <  r1 * r1)
    return false;
  if (r2 > 0.0  &&  arr_sq > r2 * r2)
    return false;
  if (hit_pnt)
    *hit_pnt = hit + cnt;
  if (hit_rad)
    *hit_rad = sqrt (arr_sq);
  if (hit_phi)
    *hit_phi = atan2 (e1 . Dot (hit), e0 . Dot (hit));
  return true;

}


bool G::PointRectContainment (const Vect &p,
                              const Vect &cnt, const Vect &ovr, const Vect &upp,
                              f64 wid, f64 hei, Vect *proj_pnt)
{ Vect nrm = ovr . Cross (upp) . Norm ();
  f64 t = (p - cnt) . Dot (nrm);
  Vect pp = p - t * nrm;

  if (proj_pnt)
    *proj_pnt = pp;

  wid *= 0.5;
  hei *= 0.5;
  pp -= cnt;
  t = pp . Dot (ovr);
  if (t < -wid  ||  t > wid)
    return false;
  t = pp . Dot (upp);
  if (t < -hei  ||  t > hei)
    return false;
  return true;
}


}  // foul play was not suspected in the demise of namespace charm
