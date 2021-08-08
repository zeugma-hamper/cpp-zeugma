
//
// (c) treadle & loam, provisioners llc
//

#ifndef CO_GRAPPLER_IS_DEDICATED_TO_THE_CONSTANCY_AND_TRANSFORMATION
#define CO_GRAPPLER_IS_DEDICATED_TO_THE_CONSTANCY_AND_TRANSFORMATION


#include "Grappler.h"


namespace zeugma  {


class CoGrappler  :  public Grappler
{ public:

  Matrix44 pm, ipm, nm, inm;

  CoGrappler ()  :  Grappler ()
    { }  // all matrices initialized to identity...

  CoGrappler (const Matrix44 &fwd_m)
     :  Grappler (), pm (fwd_m), nm (fwd_m)
    { }  // must-do: install inverted fwd_m on other two

  CoGrappler (const Matrix44 &fwd_m, const Matrix44 &bck_m)
     :  Grappler (), pm (fwd_m), ipm (bck_m), nm (fwd_m), inm (bck_m)
    { }

  CoGrappler (const Matrix44 &fwd_pm, const Matrix44 &bck_pm,
              const Matrix44 &fwd_nm, const Matrix44 &bck_nm)
     :  Grappler (), pm (fwd_pm), ipm (bck_pm), nm (fwd_nm), inm (bck_nm)
    { }


  void SetViaNormalizedBasisVectors (const Vect &e0, const Vect &e1,
                                     const Vect &e2);

  void SetViaOverAndUp (Vect o, Vect u)
    { o . NormSelf ();  u . NormSelf (); Vect n = o . Cross (u);
      SetViaNormalizedBasisVectors (o, u, n);
    }
  void SetViaOverAndNorm (Vect o, Vect n)
    { o . NormSelf ();  n . NormSelf (); Vect u = n . Cross (o);
      SetViaNormalizedBasisVectors (o, u, n);
    }
  void SetViaNormAndUp (Vect n, Vect u)
    { n . NormSelf ();  u . NormSelf (); Vect o = u . Cross (n);
      SetViaNormalizedBasisVectors (o, u, n);
    }

  Matrix44 &PointMatrix ()
    { return pm; }
  Matrix44 &InversePointMatrix ()
    { return ipm; }
  Matrix44 &NormalMatrix ()
    { return nm; }
  Matrix44 &InverseNormalMatrix ()
    { return inm; }


  void SetPointMatrix (const Matrix44 &m)
    { pm . Load (m); }
  void SetInversePointMatrix (const Matrix44 &m)
    { ipm . Load (m); }
  void SetNormalMatrix (const Matrix44 &m)
    { nm . Load (m); }
  void SetInverseNormalMatrix (const Matrix44 &m)
    { inm . Load (m); }


  const Matrix44 &PntMat ()  const  override
    { return pm; }
  const Matrix44 &NrmMat ()  const  override
    { return nm; }
  const Matrix44 &InvPntMat ()  const  override
    { return ipm; }
  const Matrix44 &InvNrmMat ()  const  override
    { return inm; }

  // no Inhale() method -- all-constant, all the time, so nothing to do!
};


}


#endif
