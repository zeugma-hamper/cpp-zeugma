
#ifndef GEOM_FUMBLE_IS_THE_UNDOING_OF_MANY_A_SENATOR
#define GEOM_FUMBLE_IS_THE_UNDOING_OF_MANY_A_SENATOR


#include "Vect.h"


namespace charm  {

#ifdef YO_COMPAT
using f64 = float64;
using namespace oblong::loam;
#endif


namespace GeomFumble
{
  bool RayPlaneIntersection (const Vect &frm, const Vect &aim,
                             const Vect &pnt, const Vect &nrm,
                             Vect *hit_pnt);
  bool LinePlaneIntersection (const Vect &frm, const Vect &aim,
                              const Vect &pnt, const Vect &nrm,
                              Vect *hit_pnt);

  bool RayRectIntersection (const Vect &frm, const Vect &aim,
                            const Vect &cnt,
                            const Vect &ovr, const Vect &upp,
                            f64 wid, f64 hei, Vect *hit_pnt);

  bool RayAnnulusIntersection (const Vect &frm, const Vect &aim,
                               const Vect &cnt, const Vect &e0,
                               const Vect &e1, f64 r1, f64 r2,
                               Vect *hit_pnt,
                               f64 *hit_rad = NULL, f64 *hit_phi = NULL);

  bool LineAnnulusIntersection (const Vect &frm, const Vect &aim,
                                const Vect &cnt, const Vect &e0,
                                const Vect &e1, f64 r1, f64 r2,
                                Vect *hit_pnt,
                                f64 *hit_rad = NULL, f64 *hit_phi = NULL);
}


}  // alas, poor namespace charm! i knew it, horatio, a fellow of infinite jest,
#endif
