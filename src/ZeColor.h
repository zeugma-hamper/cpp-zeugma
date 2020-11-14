
#ifndef ZE_COLOR_OF_POMEGRANATES
#define ZE_COLOR_OF_POMEGRANATES


#include "base_types.hpp"


namespace charm  {


class ZeColor
{ public:
  union
    { struct { f32 r, g, b, a; };
      struct { f32 c0, c1, c2, c3; };
      f32 c[4];
    };

  ZeColor ()  :  r (1.0), g (1.0), b (1.0), a (1.0)
    { }
  ZeColor (f32 gray)  :  r (gray), g (gray), b (gray), a (1.0)
    { }
  ZeColor (f32 gray, f32 alph)  :  r (gray), g (gray), b (gray), a (alph)
    { }
  ZeColor (f32 arr, f32 gee, f32 bee)  :  r (arr), g (gee), b (bee), a (1.0)
    { }
  ZeColor (f32 arr, f32 gee, f32 bee, f32 alp)
     :  r (arr), g (gee), b (bee), a (alp)
    { }
  ZeColor (const ZeColor &oth)  :  r (oth.r), g (oth.g), b (oth.b), a (oth.a)
    { }

// dirty. dirty dirty boy!
  ZeColor (ZeColor &&)             = delete;
  ZeColor &operator = (ZeColor &&) = delete;

  f32 R ()  const  { return r; }
  f32 G ()  const  { return g; }
  f32 B ()  const  { return b; }
  f32 A ()  const  { return a; }

  ZeColor &Set (f32 gray);
  ZeColor &SetNoAlpha (f32 gray);
  ZeColor &Set (f32 gray, f32 alph);
  ZeColor &Set (f32 arr, f32 gee, f32 bee);
  ZeColor &SetNoAlpha (f32 arr, f32 gee, f32 bee);
  ZeColor &Set (f32 arr, f32 gee, f32 bee, f32 alp);

  ZeColor &Set (const ZeColor &oth)
    { r = oth.r;  g = oth.g;  b = oth.b;  a = oth.a;  return *this; }
  ZeColor &SetNoAlpha (const ZeColor &oth)
    { r = oth.r;  g = oth.g;  b = oth.b;  return *this; }
  ZeColor &operator = (const ZeColor &C)
    { return Set (C); }

  ZeColor operator + (const ZeColor &C)  const
    { return ZeColor (r + C.r, g + C.g, b + C.b, a + C.a); }
  ZeColor &operator += (const ZeColor &C)
    { r += C.r;  g += C.g;  b += C.b;  a += C.a;  return *this; }

  ZeColor operator - (const ZeColor &C)  const
    { return ZeColor (r - C.r, g - C.g, b - C.b, a - C.a); }
  ZeColor &operator -= (const ZeColor &C)
    { r -= C.r;  g -= C.g;  b -= C.b;  a -= C.a;  return *this; }

  ZeColor operator * (f32 s)  const
    { return ZeColor (r*s, g*s, b*s, a*s); }
  inline friend ZeColor operator * (f32 s, const ZeColor &self)
    { return self * s; }
  ZeColor &operator *= (f32 s)
    { r *= s;  g *= s;  b *= s;  g *= s;  return *this; }

  ZeColor operator * (const ZeColor &C)  const
    { return ZeColor (r * C.r, g * C.g, b * C.b, a * C.a); }
  ZeColor &operator *= (const ZeColor &C)
    { r *= C.r;  g *= C.g;  b *= C.b;  a *= C.a;  return *this; }

  ZeColor operator / (f32 s)  const
    { s = (s == 0.0) ? 1.0 : 1.0 / s;
      return ZeColor (r*s, g*s, b*s, a*s);
    }
  ZeColor &operator /= (f32 s)
    { s = (s == 0.0) ? 1.0 : 1.0 / s;
      r *= s;  g *= s;  b *= s;  g *= s;
      return *this;
    }

  bool operator == (const ZeColor &C)  const
    { return (r == C.r  &&  g == C.g  &&  b == C.b  &&  a == C.a); }
};


}


#endif
