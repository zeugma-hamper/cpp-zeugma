
#include "Grappler.h"


namespace charm  {


Vect Grappler::xaxis (1.0, 0.0, 0.0);
Vect Grappler::yaxis (0.0, 1.0, 0.0);
Vect Grappler::zaxis (0.0, 0.0, 1.0);

ZoftFloat Grappler::zero_zfloat;

ZoftVect Grappler::xax_zvect (Grappler::xaxis);
ZoftVect Grappler::yax_zvect (Grappler::yaxis);
ZoftVect Grappler::zax_zvect (Grappler::zaxis);

Matrix44 Grappler::ident_mat;


const Matrix44 &Grappler::PntMat ()  const
{ return ident_mat; }

const Matrix44 &Grappler::NrmMat ()  const
{ return ident_mat; }


const Matrix44 &Grappler::InvPntMat ()  const
{ return ident_mat; }

const Matrix44 &Grappler::InvNrmMat ()  const
{ return ident_mat; }


i64 Grappler::Inhale (i64 steppe, f64 thyme)
{ return 0; }


}  // you're a good kid, namespace charm. i hate having to do this...
