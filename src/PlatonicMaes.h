
#ifndef PLATONIC_MAES_IS_A_GREAT_IDEA_BUT_IN_NO_WAY_PRACTICAL
#define PLATONIC_MAES_IS_A_GREAT_IDEA_BUT_IN_NO_WAY_PRACTICAL


#include "SpaceThing.h"


namespace zeugma  {


class PlatonicMaes  :  public SpaceThing
{ public:
  ZoftVect loc, ovr, upp;
  ZoftFloat wid, hei;
  i64 ideal_pixwid, ideal_pixhei;

  PlatonicMaes ()  :  SpaceThing (),
                      loc (Vect::zerov), ovr (Vect::xaxis), upp (Vect::yaxis),
                      wid (400), hei (225),
                      ideal_pixwid (-1), ideal_pixhei (-1)
    { }

  PlatonicMaes (const Vect &ell, const Vect &ohh, const Vect &yew,
                f64 dubya, f64 aytch)
     :  SpaceThing (),
        loc (ell), ovr (ohh), upp (yew), wid (dubya), hei (aytch),
        ideal_pixwid (-1), ideal_pixhei (-1)
    { }

  PlatonicMaes (const ZoftVect &lz, const ZoftVect &oz, const ZoftVect &uz,
                const ZoftFloat &wz, const ZoftFloat &hz)
     :  SpaceThing (),
        loc (lz), ovr (oz), upp (uz), wid (wz), hei (hz),
        ideal_pixwid (-1), ideal_pixhei (-1)
    { }

  PlatonicMaes (const PlatonicMaes &ma, bool follow)
     :  SpaceThing (),
        loc (follow  ?  ma.loc  :  ZoftVect (ma.loc.val)),
        ovr (follow  ?  ma.ovr  :  ZoftVect (ma.ovr.val)),
        upp (follow  ?  ma.upp  :  ZoftVect (ma.upp.val)),
        wid (follow  ?  ma.wid  :  ZoftFloat (ma.wid.val)),
        hei (follow  ?  ma.hei  :  ZoftFloat (ma.hei.val)),
        ideal_pixwid (ma.ideal_pixwid),
        ideal_pixhei (ma.ideal_pixhei)
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

  i64 IdealPixelWidth ()  const
    { return ideal_pixwid; }
  i64 IdealPixelHeight ()  const
    { return ideal_pixhei; }

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


  void SetLoc (const Vect &l)
    { loc = l; }
  void SetOver (const Vect &o)
    { ovr = o; }
  void SetUp (const Vect &u)
    { upp = u; }
  void SetWidth (f64 w)
    { wid = w; }
  void SetHeight (f64 h)
    { hei = h; }
  void SetIdealPixelWidth (i64 ipw)
    { ideal_pixwid = ipw; }
  void SetIdealPixelHeight (i64 iph)
    { ideal_pixhei = iph; }

  Vect CornerBL ()  const
    { return loc.val - 0.5 * (wid.val * ovr.val + hei.val * upp.val); }

  Vect CornerTL ()  const
    { return loc.val - 0.5 * (wid.val * ovr.val - hei.val * upp.val); }

  Vect CornerTR ()  const
    { return loc.val + 0.5 * (wid.val * ovr.val + hei.val * upp.val); }

  Vect CornerBR ()  const
    { return loc.val + 0.5 * (wid.val * ovr.val - hei.val * upp.val); }


  i64 Inhale (i64 ratch, f64 thyme);
};


}  // namespace zeugma went down to the basement to check out that weird noise

#endif
