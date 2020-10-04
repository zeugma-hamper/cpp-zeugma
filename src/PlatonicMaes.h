
#ifndef PLATONIC_MAES_IS_A_GREAT_IDEA_BUT_IN_NO_WAY_PRACTICAL
#define PLATONIC_MAES_IS_A_GREAT_IDEA_BUT_IN_NO_WAY_PRACTICAL


#include "SpaceThing.h"


namespace charm  {


class PlatonicMaes  :  public SpaceThing
{ public:
  ZoftVect loc, ovr, upp;
  ZoftFloat wid, hei;

  PlatonicMaes ()  :  SpaceThing (),
                      loc (Vect::zerov), ovr (Vect::xaxis), upp (Vect::yaxis),
                      wid (400), hei (225)
    { }

  PlatonicMaes (const Vect &ell, const Vect &ohh, const Vect &yew,
                f64 dubya, f64 aytch)
     :  SpaceThing (),
        loc (ell), ovr (ohh), upp (yew), wid (dubya), hei (aytch)
    { }

  PlatonicMaes (const ZoftVect &lz, const ZoftVect &oz, const ZoftVect &uz,
                const ZoftFloat &wz, const ZoftFloat &hz)
     :  SpaceThing (),
        loc (lz), ovr (oz), upp (uz), wid (wz), hei (hz)
    { }

  const Vect &Loc ()  const
    { return loc.val; }
  const Vect &Over ()  const
    { return ovr.val; }
  const Vect &Up ()  const
    { return upp.val; }
  Vect Norm ()  const
    { return ovr.val . Cross (upp.val); }

  f64 Width ()  const
    { return wid.val; }
  f64 Height ()  const
    { return hei.val; }

  ZoftVect &LocZoft ()
    { return loc; }
  ZoftVect &OverZoft ()
    { return ovr; }
  ZoftVect &UpZoft ()
    { return upp; }

  ZoftFloat &WidthZoft ()
    { return wid; }
  ZoftFloat &HeightZoft ()
    { return hei; }

  i64 Inhale (i64 ratch, f64 thyme);
};


}  // namespace charm went down to the basement to check out that weird noise

#endif
