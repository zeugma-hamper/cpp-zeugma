
#ifndef HASSELBLAD_EVEN_THOUGH_YOU_ARE_NOT_AN_ASTRONAUT
#define HASSELBLAD_EVEN_THOUGH_YOU_ARE_NOT_AN_ASTRONAUT


#include "SpaceThing.h"


namespace charm  {


class Bolex  :  public SpaceThing
{ public:
  enum class ProjType : u32 { PERSPECTIVE, ORTHOGRAPHIC };

  Vect view_loc;
  Vect view_aim;
  Vect view_upp;

  f64 view_dist;
  f64 view_hrz_ang_d, view_vrt_ang_d;
  f64 view_ortho_wid, view_ortho_hei;

  v2f64 view_pln_off;

  ProjType prj_typ;

  f64 near_clip_dst, far_clip_dst;

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
    { return view_loc; }
  const Vect &ViewAim ()  const
    { return view_aim; }
  Vect ViewCOI ()  const
    { return view_loc + view_dist * view_aim; }
  const Vect &ViewUp ()  const
    { return view_upp; }

  f64 ViewDist ()  const
    { return view_dist; }
  f64 ViewHorizAngleD ()  const
    { return view_hrz_ang_d; }
  f64 ViewVertAngleD ()  const
    { return view_vrt_ang_d; }
  f64 ViewHorizAngle ()  const
    { return M_PI / 180.0 * view_hrz_ang_d; }
  f64 ViewVertAngle ()  const
    { return M_PI / 180.0 * view_vrt_ang_d; }
  f64 ViewOrthoWid ()  const
    { return view_ortho_wid; }
  f64 ViewOrthoHei ()  const
    { return view_ortho_hei; }

  const v2f64 &ViewPlaneOffset ()  const
    { return view_pln_off; }
  const f64 ViewPlaneHorizOffset ()  const
    { return view_pln_off.x; }
  const f64 ViewPlaneVertOffset ()  const
    { return view_pln_off.y; }

  ProjType ProjectionType ()  const
    { return prj_typ; }

  f64 NearClipDist ()  const
    { return near_clip_dst; }
  f64 FarClipDist ()  const
    { return far_clip_dst; }


  Bolex &SetViewLoc (const Vect &loc);
  Bolex &SetViewAim (const Vect &aim);
  Bolex &SetViewCOI (const Vect &coi);
  Bolex &SetViewUp (const Vect &up);

  Bolex &SetViewDist (f64 dst);
  Bolex &SetViewHorizAngleD (f64 hdeg);
  Bolex &SetViewVertAngleD (f64 vdeg);
  Bolex &SetViewHorizAngleR (f64 hrad);
  Bolex &SetViewVertAngleR (f64 vrad);
  Bolex &SetViewOrthoWid (f64 owid);
  Bolex &SetViewOrthoHei (f64 ohei);

  Bolex &SetViewPlaneOffset (const v2f64 &off);
  Bolex &SetViewPlaneHorizOffset (f64 hoff);
  Bolex &SetViewPlaneVertOffset (f64 voff);

  Bolex &SetProjectionType (ProjType pt);

  Bolex &SetNearClipDist (f64 n_dst);
  Bolex &SetFarClipDist (f64 f_dst);
};


}  // requiem aeternum dona eis, Domine, since it's your pal namespace charm


#endif
