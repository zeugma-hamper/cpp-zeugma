
#ifndef MATRIX44_IS_LIKE_HOLLYWOOD_SQUARES_BUT_WITH_NO_PLACE_FOR_PAUL_LYNDE
#define MATRIX44_IS_LIKE_HOLLYWOOD_SQUARES_BUT_WITH_NO_PLACE_FOR_PAUL_LYNDE


#include "base_types.hpp"

#include "Vect.h"


namespace charm  {


struct InitFreeLiving { };
extern InitFreeLiving INITLESS;


class Matrix44
{ public:

  union
    { v4f64 v_[4];
      f64 a_[16];
      f64 m[4][4];
    };

  static const Matrix44 mIdent;
  static const Matrix44 mZero;

  Matrix44 ();
  explicit Matrix44 (InitFreeLiving)  { }
  Matrix44 (f64 *a);
  Matrix44 (f32 *a);
  Matrix44 (f64 a00, f64 a01, f64 a02, f64 a03,
            f64 a10, f64 a11, f64 a12, f64 a13,
            f64 a20, f64 a21, f64 a22, f64 a23,
            f64 a30, f64 a31, f64 a32, f64 a33);
  Matrix44 (const Matrix44 &otra);

  Matrix44 &LoadZero ();
  Matrix44 &LoadIdent ();

  Matrix44 &Load (f64 *a);
  Matrix44 &Load (f32 *a);
  inline Matrix44 &Load (f64 a00, f64 a01, f64 a02, f64 a03,
                         f64 a10, f64 a11, f64 a12, f64 a13,
                         f64 a20, f64 a21, f64 a22, f64 a23,
                         f64 a30, f64 a31, f64 a32, f64 a33);
  Matrix44 &Load (const Matrix44 &m);
  inline Matrix44 &operator = (const Matrix44 &m)
    { return Load (m); }

  bool operator == (const Matrix44 &m)  const;

  Matrix44 Add (const Matrix44 &m)  const;
  Matrix44 &AddToSelf (const Matrix44 &m);

  Matrix44 Sub (const Matrix44 &m)  const;
  Matrix44 &SubFromSelf (const Matrix44 &m);

  Matrix44 Mul (const Matrix44 &m)  const;
  Matrix44 &MulSelfBy (const Matrix44 &m);
  Matrix44 &PreMulSelfBy (const Matrix44 &m);

  inline Matrix44 operator + (const Matrix44 &m)  const
    { return Add (m); }
  inline Matrix44 &operator += (const Matrix44 &m)
    { return AddToSelf (m); }

  inline Matrix44 operator - (const Matrix44 &m)  const
    { return Sub (m); }
  inline Matrix44 &operator -= (const Matrix44 &m)
    { return SubFromSelf (m); }

  inline Matrix44 operator * (const Matrix44 &m)  const
    { return Mul (m); }
  inline Matrix44 &operator *= (const Matrix44 &m)
    { return MulSelfBy (m); }


  Matrix44 Transpose ()  const;
  Matrix44 &TransposeSelf ();


  Matrix44 &LoadScale (f64 s0, f64 s1, f64 s2);
  inline Matrix44 &LoadScale (f64 s)
    { return LoadScale (s, s, s); }
  Matrix44 &LoadScaleAbout (f64 s0, f64 s1, f64 s2, const Vect &c);
  inline Matrix44 &LoadScaleAbout (f64 s, const Vect &c)
    { return LoadScaleAbout (s, s, s, c); }

  Matrix44 &LoadTranslation (f64 t0, f64 t1, f64 t2);
  inline Matrix44 &LoadTranslation (const Vect &t)
    { return LoadTranslation (t.x, t.y, t.z); }

  Matrix44 &LoadRotationPreNormed (const Vect &axs, f64 ang);
  inline Matrix44 &LoadRotation (Vect axs, f64 ang)
    { axs . NormSelf ();
      return LoadRotationPreNormed (axs, ang);
    }

  Matrix44 &LoadRotationAboutPreNormed (const Vect &axs, f64 ang,
                                        const Vect &cnt);
  inline Matrix44 &LoadRotationAbout (Vect &axs, f64 ang, const Vect &cnt)
    { axs . NormSelf ();
      return LoadRotationAboutPreNormed (axs, ang, cnt);
    }

  Matrix44 &LoadCoordTransform (const Vect &o, const Vect &u, const Vect &n);
  Matrix44 &LoadCoordTransform (Vect o, Vect u)
    { o . NormSelf ();  u . NormSelf ();
      return LoadCoordTransform (o, u, o . Cross (u));
    }

  Vect TransformVect (const Vect &v)  const;
  Vect &TransformVectInPlace (Vect &v)  const;
};


}  // don't you do it! don't you die on me, namespace charm! oh well.


#endif
