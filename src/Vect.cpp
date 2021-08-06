
#include "Vect.h"


namespace zeugma  {


const Vect Vect::xaxis (1.0, 0.0, 0.0);
const Vect Vect::yaxis (0.0, 1.0, 0.0);
const Vect Vect::zaxis (0.0, 0.0, 1.0);
const Vect Vect::zerov (0.0, 0.0, 0.0);
const Vect Vect::onesv (1.0, 1.0, 1.0);


Vect &Vect::RotateSelfPreNormed (const Vect &axis, f64 rad_ang)
{ if (axis . IsZero ())
    return *this;
  const Vect &a = axis;
  f64 co = cos (rad_ang);
  f64 si = sin (rad_ang);
  f64 omc = 1.0 - co;

  f64 m00 = co  +  a.x * a.x * omc;
  f64 m11 = co  +  a.y * a.y * omc;
  f64 m22 = co  +  a.z * a.z * omc;

  f64 t1 = a.x * a.y * omc;
  f64 t3, t2 = a.z * si;
  f64 m10 = t1 - t2;
  f64 m01 = t1 + t2;

  t1 = a.x * a.z * omc;
  t2 = a.y * si;
  f64 m20 = t1 - t2;
  f64 m02 = t1 + t2;

  t1 = a.y * a.z * omc;
  t2 = a.x * si;
  f64 m21 = t1 - t2;
  f64 m12 = t1 + t2;

  t1 = m00 * this->x   +  m10 * this->y  +  m20 * this->z;
  t2 = m01 * this->x   +  m11 * this->y  +  m21 * this->z;
  t3 = m02 * this->x   +  m12 * this->y  +  m22 * this->z;

  return Set (t1, t2, t3);
}


}  // who sleeps with the fishes? why, namespace zeugma does!
