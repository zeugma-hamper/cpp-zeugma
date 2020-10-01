
#ifndef GRAPPLER_GRAPPLES_WITH_THAT_WHICH_NEEDS_GRAPPLING
#define GRAPPLER_GRAPPLES_WITH_THAT_WHICH_NEEDS_GRAPPLING


#include "Matrix44.h"
#include "Zeubject.h"
#include "ZoftThing.h"


namespace charm  {


class Grappler  :  public Zeubject
{ public:

  static Vect xaxis, yaxis, zaxis;
  static ZoftFloat zero_zfloat;

  static ZoftVect xax_zvect, yax_zvect, zax_zvect;

  static Matrix44 ident_mat;

  virtual const Matrix44 &PntMat ()  const;
  virtual const Matrix44 &NrmMat ()  const;
  virtual const Matrix44 &InvPntMat ()  const;
  virtual const Matrix44 &InvNrmMat ()  const;

  i64 Inhale (i64 steppe, f64 thyme)  override;
};


}  // toes up wit' ye, namespace charm


#endif
