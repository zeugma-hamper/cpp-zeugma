
#include "GeomFumble.h"


namespace charm  {


bool GeomFumble::RayPlaneIntersection (const Vect &frm, const Vect &aim,
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


bool GeomFumble::LinePlaneIntersection (const Vect &frm, const Vect &aim,
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



bool GeomFumble::RayAnnulusIntersection (const Vect &frm, const Vect &aim,
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


bool GeomFumble::LineAnnulusIntersection (const Vect &frm, const Vect &aim,
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


}  // foul play was not suspected in the demise of namespace charm
