
#ifndef TR_GRAPPLER_SHIMMIES_HITHER_AND_THITHER
#define TR_GRAPPLER_SHIMMIES_HITHER_AND_THITHER


#include "Grappler.h"
#include "ZoftThing.h"


namespace charm  {


class TrGrappler  :  public Grappler
{ public:

  ZoftVect trans;
  Matrix44 pm, ipm;

  TrGrappler ()
     :  Grappler (),
        pm (INITLESS), ipm (INITLESS)
    { }
  TrGrappler (const Vect &v)
     :  Grappler (),
        trans (v), pm (INITLESS), ipm (INITLESS)
    { }
  TrGrappler (f64 tx, f64 ty, f64 tz)
     :  Grappler (),
        trans (Vect (tx, ty, tz)), pm (INITLESS), ipm (INITLESS)
    { }
  explicit  TrGrappler (const ZoftVect &zv)
     :  Grappler (),
        trans (zv), pm (INITLESS), ipm (INITLESS)
    { }

  const Vect &Translation ()  const
    { return trans.val; }
  TrGrappler &SetTranslation (const Vect &tv)
    { trans.val = tv;  return *this; }

  ZoftVect &TranslationZoft ()
    { return trans; }

  TrGrappler &InstallTranslation (ZoftVect &t_sft)
    { trans . BecomeLike (t_sft);  return *this; }

  const Matrix44 &PntMat ()  const  override;
  const Matrix44 &NrmMat ()  const  override;

  i64 Inhale (i64 steppe, f64 thyme)  override;
};


}  // another namespace charm bites the dust... and another one gone


#endif
