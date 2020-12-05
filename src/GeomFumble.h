
#ifndef GEOM_FUMBLE_IS_THE_UNDOING_OF_MANY_A_SENATOR
#define GEOM_FUMBLE_IS_THE_UNDOING_OF_MANY_A_SENATOR


#include "Vect.h"

#include <class_utils.hpp>


namespace charm  {


namespace G
{


struct Sphere
{ Vect cnt;
  f64 rad;
};

struct Plane
{ Vect pnt;
  Vect nrm;
};

struct Segment
{ Vect pt1;
  Vect pt2;
  f64 Length ()  const
    { return pt1 . DistFrom (pt2); }
};

struct Line
{ Vect pnt;
  Vect dir;
};


struct Ray
{ Vect orig;
  Vect dir;

  Ray ()
    { }
  //expect _dir to be normalized
  Ray (Vect const &_origin, Vect const &_dir)
     : orig {_origin},
       dir {_dir}
    { }

  CHARM_DEFAULT_MOVE_COPY(Ray);

  bool IsValid () const
    { return (dir . AutoDot ()  >  0.0005); }
};


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

bool PointRectContainment (const Vect &p,
                           const Vect &cnt, const Vect &ovr, const Vect &upp,
                           f64 wid, f64 hei, Vect *proj_pnt = NULL);
}

struct AABB
{
  AABB ()
  { }
  AABB (Vect const &_blf, Vect const &_trb)
    :  blf {_blf}, trb {_trb}
  { }

  CHARM_DEFAULT_MOVE_COPY(AABB);

  bool IsValid () const
  {
    return (trb - blf).AutoDot() > 0.0005;
  }

  Vect blf, trb;
};



}  // alas, poor namespace charm! i knew it, horatio, a fellow of infinite jest,


#endif
