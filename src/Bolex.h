
#ifndef HASSELBLAD_EVEN_THOUGH_YOU_ARE_NOT_AN_ASTRONAUT
#define HASSELBLAD_EVEN_THOUGH_YOU_ARE_NOT_AN_ASTRONAUT


#include "SpaceThing.h"

#include "ZoftThing.h"


namespace charm  {


class Bolex  :  public SpaceThing
{ public:
  enum class ProjType : u32 { PERSPECTIVE, ORTHOGRAPHIC };

  ZoftVect view_loc;
  ZoftVect view_aim;
  ZoftVect view_upp;

  ZoftFloat view_dist;
  ZoftFloat view_hrz_ang_d;
  ZoftFloat view_vrt_ang_d;
  ZoftFloat view_ortho_wid;
  ZoftFloat view_ortho_hei;

  v2f64 view_pln_off;

  ProjType prj_typ;

  ZoftFloat near_clip_dst;
  ZoftFloat far_clip_dst;

  static Vect xax;
  static Vect yax;
  static Vect zax;

  Bolex ()  :  SpaceThing (),
     view_loc (Vect (0.0, 0.0, 100.0)), view_aim (-zax), view_upp (yax),
     view_dist (100.0), view_hrz_ang_d (75.0), view_vrt_ang_d (46.69),
     view_ortho_wid (160.0), view_ortho_hei (90.0),
     view_pln_off { 0.0, 0.0 },
     prj_typ (ProjType::PERSPECTIVE),
     near_clip_dst (0.01), far_clip_dst (1000.0)
    { }

  const Vect &ViewLoc ()  const
    { return view_loc.val; }
  const Vect &ViewAim ()  const
    { return view_aim.val; }
  Vect ViewCOI ()  const
    { return view_loc.val + view_dist.val * view_aim.val; }
  const Vect &ViewUp ()  const
    { return view_upp.val; }

  f64 ViewDist ()  const
    { return view_dist.val; }
  f64 ViewHorizAngleD ()  const
    { return view_hrz_ang_d.val; }
  f64 ViewVertAngleD ()  const
    { return view_vrt_ang_d.val; }
  f64 ViewHorizAngle ()  const
    { return M_PI / 180.0 * view_hrz_ang_d.val; }
  f64 ViewVertAngle ()  const
    { return M_PI / 180.0 * view_vrt_ang_d.val; }
  f64 ViewOrthoWid ()  const
    { return view_ortho_wid.val; }
  f64 ViewOrthoHei ()  const
    { return view_ortho_hei.val; }

  const v2f64 &ViewPlaneOffset ()  const
    { return view_pln_off; }
  f64 ViewPlaneHorizOffset ()  const
    { return view_pln_off.x; }
  f64 ViewPlaneVertOffset ()  const
    { return view_pln_off.y; }

  ProjType ProjectionType ()  const
    { return prj_typ; }
  bool IsPerspectiveTypeProjection ()  const
    { return prj_typ == ProjType::PERSPECTIVE; }
  bool IsPerspectiveTypeOthographic ()  const
    { return prj_typ == ProjType::ORTHOGRAPHIC; }

  f64 NearClipDist ()  const
    { return near_clip_dst.val; }
  f64 FarClipDist ()  const
    { return far_clip_dst.val; }


  ZoftVect &ViewLocZoft ()
    { return view_loc; }
  ZoftVect &ViewAimZoft ()
    { return view_aim; }
  ZoftVect &ViewUpZoft ()
    { return view_upp; }

  ZoftFloat &ViewDistZoft ()
    { return view_dist; }

  ZoftFloat &ViewHorizAngleDZoft ()
    { return view_hrz_ang_d; }
  ZoftFloat &ViewVertAngleDZoft ()
    { return view_vrt_ang_d; }

  ZoftFloat &ViewOrthoWidZoft ()
    { return view_ortho_wid; }
  ZoftFloat &ViewOrthoHeiZoft ()
    { return view_ortho_hei; }

  ZoftFloat &NearClipDistZoft ()
    { return near_clip_dst; }
  ZoftFloat &FarClipDistZoft ()
    { return far_clip_dst; }


  Bolex &SetViewLoc (const Vect &loc);
  Bolex &SetViewAim (const Vect &aim);
  Bolex &SetViewCOI (const Vect &coi);
  Bolex &SetViewUp (const Vect &up);

  Bolex &SetViewDist (f64 dst);
  Bolex &SetViewHorizAngleD (f64 hdeg);
  Bolex &SetViewVertAngleD (f64 vdeg);
  Bolex &SetViewHorizAngle (f64 hrad);
  Bolex &SetViewVertAngle (f64 vrad);
  Bolex &SetViewOrthoWid (f64 owid);
  Bolex &SetViewOrthoHei (f64 ohei);

  Bolex &SetViewPlaneOffset (const v2f64 &off);
  Bolex &SetViewPlaneHorizOffset (f64 hoff);
  Bolex &SetViewPlaneVertOffset (f64 voff);

  Bolex &SetProjectionType (ProjType pt);

  Bolex &SetNearClipDist (f64 n_dst);
  Bolex &SetFarClipDist (f64 f_dst);
  Bolex &SetNearAndFarClipDist (f64 n_dst, f64 f_dst);
};


}  // requiem aeternum dona eis, Domine, since it's your pal namespace charm


#endif
