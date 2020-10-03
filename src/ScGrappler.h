
#ifndef SC_GRAPPLER_HAS_ITS_THUMB_ON_THE_PAN
#define SC_GRAPPLER_HAS_ITS_THUMB_ON_THE_PAN


#include "Grappler.h"
#include "ZoftThing.h"


namespace charm  {


class ScGrappler  :  public Grappler
{ public:

  ZoftVect cnt, sca;
  Matrix44 pm, ipm, nm, inm;

  ScGrappler ()
     :  Grappler (),
        sca (Vect (1.0, 1.0, 1.0)),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }
  ScGrappler (const ZoftVect &s_sft)
     :  Grappler (),
        sca (s_sft),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }
  ScGrappler (const ZoftVect &s_sft, const ZoftVect &c_sft)
     :  Grappler (),
        cnt (c_sft), sca (s_sft),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }
  ScGrappler (f64 s)
     :  Grappler (),
        sca (Vect (s, s, s)),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }
  ScGrappler (f64 s, const Vect &c)
     :  Grappler (),
        cnt (c), sca (Vect (s, s, s)),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }
  ScGrappler (f64 sx, f64 sy, f64 sz)
     :  Grappler (),
        sca (Vect (sx, sy, sz)),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }
  ScGrappler (const Vect &s, const Vect &c)
     :  Grappler (),
        cnt (c), sca (s),
        pm (INITLESS), ipm (INITLESS), nm (INITLESS), inm (INITLESS)
    { }

  const Vect &Scale ()  const
    { return sca.val; }
  const Vect &Center ()  const
    { return cnt.val; }

  ZoftVect &ScaleZoft ()
    { return sca; }
  ZoftVect &CenterZoft ()
    { return cnt; }

  ScGrappler &InstallScale (const ZoftVect &s_sft)
    { sca . BecomeLike (s_sft);  return *this; }
  ScGrappler &InstallCenter (const ZoftVect &c_sft)
    { cnt . BecomeLike (c_sft);  return *this; }

  const Matrix44 &PntMat ()  const  override
    { return pm; }
  const Matrix44 &NrmMat ()  const  override
    { return nm; }
  const Matrix44 &InvPntMat ()  const  override
    { return ipm; }
  const Matrix44 &InvNrmMat ()  const  override
    { return inm; }

  i64 Inhale (i64 steppe, f64 thyme)  override;
};


}  // superman can fly backwards around the earth; won't help namespace charm


#endif
