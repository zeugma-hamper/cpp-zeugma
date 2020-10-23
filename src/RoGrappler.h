
#ifndef RO_GRAPPLER_SPINS_YOU_RIGHT_ROUND_BABY_RIGHT_OR_LEFT
#define RO_GRAPPLER_SPINS_YOU_RIGHT_ROUND_BABY_RIGHT_OR_LEFT


#include "Grappler.h"
#include "ZoftThing.h"


namespace charm  {


class RoGrappler  :  public Grappler
{ public:

  ZoftVect axs, cnt;
  ZoftFloat ang, pha;
  Matrix44 pm, ipm, nm, inm;

  RoGrappler (const Vect &ax, f64 an = 0.0,
              const Vect &ce = Vect (), f64 ph = 0.0)
     : Grappler (),
       axs (ax), cnt (ce), ang (an), pha (ph)
    { }
  RoGrappler (const ZoftVect &ax_zft,
              const ZoftFloat &an_zft = zero_zfloat,
              const ZoftVect &ce_zft = zax_zvect,
              const ZoftFloat &ph_zft = zero_zfloat)
     :  Grappler (),
        axs (ax_zft), cnt (ce_zft), ang (an_zft), pha (ph_zft)
    { }

  const Vect &Axis ()  const
    { return axs.val; }
  const Vect &Center ()  const
    { return cnt.val; }
  f64 Angle ()  const
    { return ang.val; }
  f64 AngleD ()  const
    { return 180.0 / M_PI * ang.val; }
  f64 Phase ()  const
    { return pha.val; }

  ZoftVect &AxisZoft ()
    { return axs; }
  ZoftFloat &AngleZoft ()
    { return ang; }
  ZoftVect &CenterZoft ()
    { return cnt; }
  ZoftFloat &PhaseZoft ()
    { return pha; }

  RoGrappler &InstallAxis (ZoftVect &a)
    { axs . BecomeLike (a);  return *this; }
  RoGrappler &InstallAngle (ZoftFloat &a)
    { ang . BecomeLike (a);  return *this; }
  RoGrappler &InstallCenter (ZoftVect &c)
    { cnt . BecomeLike (c);  return *this; }
  RoGrappler &InstallPhase (ZoftFloat &p)
    { pha . BecomeLike (p);  return *this; }

  RoGrappler &InstallAxis (const ZoftVect &a)
    { axs . BecomeLike (a);  return *this; }
  RoGrappler &InstallAngle (const ZoftFloat &a)
    { ang . BecomeLike (a);  return *this; }
  RoGrappler &InstallCenter (const ZoftVect &c)
    { cnt . BecomeLike (c);  return *this; }
  RoGrappler &InstallPhase (const ZoftFloat &p)
    { pha . BecomeLike (p);  return *this; }

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


}  // end of the line for namespace charm


#endif
