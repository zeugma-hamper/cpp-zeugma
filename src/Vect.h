
#ifndef VECTITUDINOUSNESS
#define VECTITUDINOUSNESS

#include <base_types.hpp>

#include <math.h>
#include <stdio.h>

namespace charm
{

class Vect  :  public v3f64
{ public:
  Vect ()  { x = y = z = 0.0; }
  Vect (f64 eks, f64 wye, f64 zee)
    { x = eks;  y = wye;  z = zee; }
  Vect (f64 mono)
    { x = y = z = mono; }

  Vect (const Vect &otra) noexcept = default;
  Vect (Vect &&otra) noexcept = default;

  Vect &operator = (const Vect &otra) noexcept = default;
  Vect &operator = (Vect &&otra) noexcept = default;

  Vect &Set (f64 eks, f64 wye, f64 zee)
    { x = eks;  y = wye;  z = zee;  return *this; }
  Vect &Set (f64 mono)
    { x = y = z = mono;  return *this; }
  Vect &Set (const Vect &otra)
    { x = otra.x;  y = otra.y;  z = otra.z;  return *this; }


  bool operator == (const Vect &v)  const
    { return (x == v.x  &&  y == v.y  &&  z == v.z); }


  f64 Dot (const Vect &v)  const
    { return (x * v.x  +  y * v.y  +  z * v.z); }

  f64 AutoDot ()  const
    { return (x*x + y*y + z*z); }

  f64 Mag ()  const
    { return sqrt (AutoDot ()); }

  Vect Cross (const Vect &v)  const
    { return Vect (y * v.z  -  z * v.y,
                   z * v.x  -  x * v.z,
                   x * v.y  -  y * v.x);
    }

  Vect operator - ()  const
    { return Vect (-x, -y, -z); }

  Vect operator + (const Vect &v)  const
    { return Vect (x + v.x,  y + v.y,  z + v.z); }
  Vect &operator += (const Vect &v)
    { x += v.x;  y += v.y;  z += v.z;  return *this; }

  Vect operator - (const Vect &v)  const
    { return Vect (x - v.x,  y - v.y,  z - v.z); }
  Vect &operator -= (const Vect &v)
    { x -= v.x;  y -= v.y;  z -= v.z;  return *this; }

  Vect operator * (f64 s)  const
    { return Vect (s*x, s*y, s*z); }
  friend Vect operator * (f64 s, const Vect &v)
    { return v * s; }
  Vect operator *= (f64 s)
    { x *= s;  y *= s;  z *= s;  return *this; }

  Vect operator / (f64 s)  const
    { return Vect (s/x, s/y, s/z); }
  Vect operator /= (f64 s)
    { x /= s;  y /= s;  z /= s;  return *this; }

  f64 DistFrom (const Vect &v)  const
    { return sqrt ((*this - v) . AutoDot ()); }

  bool IsZero ()  const
    { return (x == 0.0  &&  y == 0.0  &&  z == 0.0); }
  Vect &Zero ()
    { return Set (0.0, 0.0, 0.0); }

  Vect Norm ()  const
    { f64 s = Mag ();
      if (s == 0.0)
        return Vect ();
      s = 1.0 / s;
      return Vect (s*x, s*y, s*z);
    }
  Vect &NormSelf ()
    { f64 s = Mag ();
      if (s != 0.0)
        { x *= (s = 1.0 / s);  y *= s;  z *= s; }
      return *this;
    }

  f64 AngleWith (const Vect &v)  const
    { return acos (Norm () . Dot (v . Norm ())); }
  inline f64 DegAngleWith (const Vect &v)  const
    { return 180.0 / M_PI * AngleWith (v); }

  Vect &RotateSelfPreNormed (const Vect &axis, f64 rad_ang);
  inline Vect &RotateSelf (Vect axis, f64 rad_ang)
    { axis . NormSelf ();
      return RotateSelfPreNormed (axis, rad_ang);
    }
  Vect RotatePreNormed (const Vect &axis, f64 rad_ang)  const
    { Vect outv = *this;
      outv . RotateSelfPreNormed (axis, rad_ang);
      return outv;
    }
  Vect Rotate (const Vect &axis, f64 rad_ang)  const
    { Vect outv = *this;
      outv . RotateSelf (axis, rad_ang);
      return outv;
    }

  Vect RotateAbout (const Vect &axis, f64 rad_ang, const Vect &cent)  const
    { return ((*this - cent) . Rotate (axis, rad_ang)  +  cent); }
  Vect &RotateSelfAbout (const Vect &axis, f64 rad_ang, const Vect &cent)
    { *this -= cent;  RotateSelf (axis, rad_ang);
      *this += cent;  return *this;
    }

  Vect RotateAboutPreNormed (const Vect &axis, f64 rad_ang,
                             const Vect &cent)  const
    { return ((*this - cent) . RotatePreNormed (axis, rad_ang)  +  cent); }
  Vect &RotateSelfAboutPreNormed (const Vect &axis, f64 rad_ang,
                                  const Vect &cent)
    { *this -= cent;  RotateSelfPreNormed (axis, rad_ang);
      *this += cent;  return *this;
    }

  void Spew (FILE *unto)  const
    { fprintf (unto, "V3<%.2lf %.2lf %.2lf>", x, y, z); }
  void SpewToStderr ()  const
    { Spew (stderr); }
};


#define VEX(v) (v).x, (v).y, (v).z
#define PVEX(pv) (pv)->x, (pv)->y, (pv)->z

#define VEX2(v) (v).x, (v).y
#define PVEX2(pv) (pv)->x, (pv)->y

}

#endif
