
//
// (c) treadle & loam, provisioners llc
//

#include "Matrix44.h"


namespace zeugma  {


const Matrix44 Matrix44::mIdent (INITLESS);

const Matrix44 Matrix44::mZero (INITLESS );

bool _Matrix44_beget_static_items ()
{ static bool well_and_truly_done = false;
  if (! well_and_truly_done)
    { Matrix44 *mi = const_cast <Matrix44 *> (&Matrix44::mIdent);
      mi -> Load ( 1.0, 0.0, 0.0, 0.0,
                   0.0, 1.0, 0.0, 0.0,
                   0.0, 0.0, 1.0, 0.0,
                   0.0, 0.0, 0.0, 1.0 );
      Matrix44 *mz = const_cast <Matrix44 *> (&Matrix44::mZero);
      mz -> Load  ( 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0 );
      well_and_truly_done = true;
    }
  return well_and_truly_done;
}


#define MPT_CPY(PFR, PTO, TFR, TTO)             \
  do                                            \
    { const TFR *fr = (PFR) + 16;               \
      TTO *bot = (PTO);                         \
      TTO *to = bot + 16;                       \
      do { *--to = *--fr; }                     \
      while (to  >  bot);                       \
    }                                           \
  while (0)

#define MP_CPY(PFR, PTO)                        \
  MPT_CPY (PFR, PTO, f64, f64)

#define MP32_CPY(PFR, PTO)                      \
  MPT_CPY (PFR, PTO, f32, f64)

#define M_CPY(MFR, MTO)                         \
  MP_CPY (&((MFR).a_[0]), &((MTO).a_[0]))



Matrix44::Matrix44 ()
{ static bool got_this_party_started = _Matrix44_beget_static_items ();
  M_CPY (mIdent, *this);
}


Matrix44::Matrix44 (f64 *a)
{ MP_CPY (a, &a_[0]); }


Matrix44::Matrix44 (f32 *a)
{ MP32_CPY (a, &a_[0]); }


Matrix44::Matrix44 (f64 a00, f64 a01, f64 a02, f64 a03,
                    f64 a10, f64 a11, f64 a12, f64 a13,
                    f64 a20, f64 a21, f64 a22, f64 a23,
                    f64 a30, f64 a31, f64 a32, f64 a33)
{ m[0][0] = a00;  m[0][1] = a01;  m[0][2] = a02;  m[0][3] = a03;
  m[1][0] = a10;  m[1][1] = a11;  m[1][2] = a12;  m[1][3] = a13;
  m[2][0] = a20;  m[2][1] = a21;  m[2][2] = a22;  m[2][3] = a23;
  m[3][0] = a30;  m[3][1] = a31;  m[3][2] = a32;  m[3][3] = a33;
}


Matrix44::Matrix44 (const Matrix44 &otra)
{ M_CPY (otra, *this); }



Matrix44 &Matrix44::LoadZero ()
{ static bool got_this_party_started = _Matrix44_beget_static_items ();
  M_CPY (mZero, *this);
  return *this;
}

Matrix44 &Matrix44::LoadIdent ()
{ static bool got_this_party_started = _Matrix44_beget_static_items ();
  M_CPY (mIdent, *this);
  return *this;
}


Matrix44 &Matrix44::Load (f64 *a)
{ MP_CPY (a, &a_[0]);
  return *this;
}

Matrix44 &Matrix44::Load (f32 *a)
{ MP32_CPY (a, &a_[0]);
  return *this;
}

Matrix44 &Matrix44::Load (f64 a00, f64 a01, f64 a02, f64 a03,
                          f64 a10, f64 a11, f64 a12, f64 a13,
                          f64 a20, f64 a21, f64 a22, f64 a23,
                          f64 a30, f64 a31, f64 a32, f64 a33)
{ m[0][0] = a00;  m[0][1] = a01;  m[0][2] = a02;  m[0][3] = a03;
  m[1][0] = a10;  m[1][1] = a11;  m[1][2] = a12;  m[1][3] = a13;
  m[2][0] = a20;  m[2][1] = a21;  m[2][2] = a22;  m[2][3] = a23;
  m[3][0] = a30;  m[3][1] = a31;  m[3][2] = a32;  m[3][3] = a33;
  return *this;
}

Matrix44 &Matrix44::Load (const Matrix44 &otra)
{ M_CPY (otra, *this);
  return *this;
}


bool Matrix44::operator == (const Matrix44 &m)  const
{ const f64 *e = &a_[0];
  const f64 *a = e + 16;
  const f64 *b = &m.a_[0] + 16;
  do
    if (*--a  !=  *--b)
      return false;
  while (a  >=  e);
  return true;
}


Matrix44 Matrix44::Add (const Matrix44 &m)  const
{ Matrix44 res;
  f64 *c = &res.a_[0] + 16;
  const f64 *e = &a_[0];
  const f64 *a = e + 16;
  const f64 *b = &m.a_[0] + 16;
  do
    { *--c  =  *--a  +  *--b; }
  while (a  >=  e);
  return res;
}

Matrix44 &Matrix44::AddToSelf (const Matrix44 &m)
{ f64 *e = &a_[0];
  f64 *a = e + 16;
  const f64 *b = &m.a_[0] + 16;
  do
    { *--a  +=  *--b; }
  while (a  >=  e);
  return *this;
}


Matrix44 Matrix44::Sub (const Matrix44 &m)  const
{ Matrix44 res;
  f64 *c = &res.a_[0] + 16;
  const f64 *e = &a_[0];
  const f64 *a = e + 16;
  const f64 *b = &m.a_[0] + 16;
  do
    { *--c  =  *--a  -  *--b; }
  while (a  >=  e);
  return res;
}

Matrix44 &Matrix44::SubFromSelf (const Matrix44 &m)
{ f64 *e = &a_[0];
  f64 *a = e + 16;
  const f64 *b = &m.a_[0] + 16;
  do
    { *--a  -=  *--b; }
  while (a  >=  e);
  return *this;
}


#define RCDOT(i, j)  (ma[i][0] * mb[0][j]  +  ma[i][1] * mb[1][j]  +  \
                      ma[i][2] * mb[2][j]  +  ma[i][3] * mb[3][j])

Matrix44 Matrix44::Mul (const Matrix44 &m)  const
{ auto &ma = this->m;
  auto &mb = m.m;
  return Matrix44 (RCDOT(0,0), RCDOT(0,1), RCDOT(0,2), RCDOT(0,3),
                   RCDOT(1,0), RCDOT(1,1), RCDOT(1,2), RCDOT(1,3),
                   RCDOT(2,0), RCDOT(2,1), RCDOT(2,2), RCDOT(2,3),
                   RCDOT(3,0), RCDOT(3,1), RCDOT(3,2), RCDOT(3,3));
}

Matrix44 &Matrix44::MulSelfBy (const Matrix44 &m)
{ auto &ma = this->m;
  auto &mb = m.m;
  return Load (RCDOT(0,0), RCDOT(0,1), RCDOT(0,2), RCDOT(0,3),
               RCDOT(1,0), RCDOT(1,1), RCDOT(1,2), RCDOT(1,3),
               RCDOT(2,0), RCDOT(2,1), RCDOT(2,2), RCDOT(2,3),
               RCDOT(3,0), RCDOT(3,1), RCDOT(3,2), RCDOT(3,3));
}

Matrix44 &Matrix44::PreMulSelfBy (const Matrix44 &m)
{ auto &ma = m.m;
  auto &mb = this->m;
  return Load (RCDOT(0,0), RCDOT(0,1), RCDOT(0,2), RCDOT(0,3),
               RCDOT(1,0), RCDOT(1,1), RCDOT(1,2), RCDOT(1,3),
               RCDOT(2,0), RCDOT(2,1), RCDOT(2,2), RCDOT(2,3),
               RCDOT(3,0), RCDOT(3,1), RCDOT(3,2), RCDOT(3,3));
}



Matrix44 Matrix44::Transpose ()  const
{ return Matrix44 (m[0][0], m[1][0], m[2][0], m[3][0],
                   m[0][1], m[1][1], m[2][1], m[3][1],
                   m[0][2], m[1][2], m[2][2], m[3][2],
                   m[0][3], m[1][3], m[2][3], m[3][3]);
}


#define SWP(i,j)  t = em[i][j];  em[i][j] = em[j][i];  em[j][i] = t

Matrix44 &Matrix44::TransposeSelf ()
{ f64 t;
  auto &em = m;
  SWP(0,1);
  SWP(0,2); SWP(1,2);
  SWP(0,3); SWP(1,3); SWP(2,3);
  return *this;
}



Matrix44 &Matrix44::LoadScale (f64 s0, f64 s1, f64 s2)
{ M_CPY (mIdent, *this);
  m[0][0] = s0;
  m[1][1] = s1;
  m[2][2] = s2;
  return *this;
}

Matrix44 &Matrix44::LoadScaleAbout (f64 s0, f64 s1, f64 s2, const Vect &c)
{ M_CPY (mIdent, *this);
  m[0][0] = s0;
  m[1][1] = s1;
  m[2][2] = s2;
  m[3][0] = c.x * (1.0 - s0);
  m[3][1] = c.y * (1.0 - s1);
  m[3][2] = c.z * (1.0 - s2);
  return *this;
}


Matrix44 &Matrix44::LoadTranslation (f64 t0, f64 t1, f64 t2)
{ M_CPY (mIdent, *this);
  m[3][0] = t0;
  m[3][1] = t1;
  m[3][2] = t2;
  return *this;
}


Matrix44 &Matrix44::LoadRotationPreNormed (const Vect &axs, f64 ang)
{ if (axs . IsZero ())
    return LoadIdent ();  // arguably a rational response; else throw up?
  f64 co = cos (ang);
  f64 si = sin (ang);
  f64 omc = 1.0 - co;

  f64 m00 = co  +  axs.x * axs.x * omc;
  f64 m11 = co  +  axs.y * axs.y * omc;
  f64 m22 = co  +  axs.z * axs.z * omc;

  f64 t1 = axs.x * axs.y * omc;
  f64 t2 = axs.z * si;
  f64 m10 = t1 - t2;
  f64 m01 = t1 + t2;

  t1 = axs.x * axs.z * omc;
  t2 = axs.y * si;
  f64 m20 = t1 + t2;
  f64 m02 = t1 - t2;

  t1 = axs.y * axs.z * omc;
  t2 = axs.x * si;
  f64 m21 = t1 - t2;
  f64 m12 = t1 + t2;

  return Load (m00, m01, m02, 0.0,
               m10, m11, m12, 0.0,
               m20, m21, m22, 0.0,
               0.0, 0.0, 0.0, 1.0);
}


Matrix44 &Matrix44::LoadRotationAboutPreNormed (const Vect &axs, f64 ang,
                                                const Vect &cnt)
{ LoadTranslation (-cnt.x, -cnt.y, -cnt.z);
  Matrix44 em (INITLESS);
  em . LoadRotationPreNormed (axs, ang);
  MulSelfBy (em);
  em . LoadTranslation (cnt.x, cnt.y, cnt.z);
  return MulSelfBy (em);
}



Matrix44 &Matrix44::LoadCoordTransform (const Vect &o, const Vect &u,
                                        const Vect &n)
{ M_CPY (mIdent, *this);
  m[0][0] = o.x;  m[0][1] = o.y;  m[0][2] = o.z;
  m[1][0] = u.x;  m[1][1] = u.y;  m[1][2] = u.z;
  m[2][0] = n.x;  m[2][1] = n.y;  m[2][2] = n.z;
  return *this;
}



Vect Matrix44::TransformVect (const Vect &v)  const
{ f64 a0 = v.x * m[0][0]  +  v.y * m[1][0]  +  v.z * m[2][0]  +  m[3][0];
  f64 a1 = v.x * m[0][1]  +  v.y * m[1][1]  +  v.z * m[2][1]  +  m[3][1];
  f64 a2 = v.x * m[0][2]  +  v.y * m[1][2]  +  v.z * m[2][2]  +  m[3][2];
  return Vect (a0, a1, a2);
}

Vect &Matrix44::TransformVectInPlace (Vect &v)  const
{ f64 a0 = v.x * m[0][0]  +  v.y * m[1][0]  +  v.z * m[2][0]  +  m[3][0];
  f64 a1 = v.x * m[0][1]  +  v.y * m[1][1]  +  v.z * m[2][1]  +  m[3][1];
  f64 a2 = v.x * m[0][2]  +  v.y * m[1][2]  +  v.z * m[2][2]  +  m[3][2];
  return v . Set (a0, a1, a2);
}


}  // a time to live, a time to... well, you know the rest, namespace zeugma
