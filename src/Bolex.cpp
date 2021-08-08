
//
// (c) treadle & loam, provisioners llc
//

#include "Bolex.h"


namespace zeugma  {



Vect Bolex::xax (1.0, 0.0, 0.0);
Vect Bolex::yax (0.0, 1.0, 0.0);
Vect Bolex::zax (0.0, 0.0, 1.0);



Bolex &Bolex::SetViewLoc (const Vect &loc)
{ view_loc = loc;
  return *this;
}

Bolex &Bolex::SetViewAim (const Vect &aim)
{ view_aim = aim;
  return *this;
}

Bolex &Bolex::SetViewCOI (const Vect &coi)
{ view_aim = (coi - view_loc) . Norm ();
  return *this;
}

Bolex &Bolex::SetViewUp (const Vect &up)
{ view_upp = up;
  return *this;
}


Bolex &Bolex::SetViewDist (f64 dst)
{ view_dist = dst;
  return *this;
}

Bolex &Bolex::SetViewHorizAngleD (f64 hdeg)
{ view_hrz_ang_d = hdeg;
  return *this;
}

Bolex &Bolex::SetViewVertAngleD (f64 vdeg)
{ view_vrt_ang_d = vdeg;
  return *this;
}

Bolex &Bolex::SetViewHorizAngle (f64 hrad)
{ view_hrz_ang_d = hrad * 180.0 / M_PI;
  return *this;
}

Bolex &Bolex::SetViewVertAngle (f64 vrad)
{ view_vrt_ang_d = vrad * 180.0 / M_PI;
  return *this;
}

Bolex &Bolex::SetViewOrthoWid (f64 owid)
{ view_ortho_wid = owid;
  return *this;
}

Bolex &Bolex::SetViewOrthoHei (f64 ohei)
{ view_ortho_hei = ohei;
  return *this;
}


Bolex &Bolex::SetViewPlaneOffset (const v2f64 &off)
{ view_pln_off = off;
  return *this;
}

Bolex &Bolex::SetViewPlaneHorizOffset (f64 hoff)
{ view_pln_off.x = hoff;
  return *this;
}

Bolex &Bolex::SetViewPlaneVertOffset (f64 voff)
{ view_pln_off.y = voff;
  return *this;
}


Bolex &Bolex::SetProjectionType (ProjType pt)
{ prj_typ = pt;
  return *this;
}


Bolex &Bolex::SetNearClipDist (f64 n_dst)
{ near_clip_dst = n_dst;
  return *this;
}

Bolex &Bolex::SetFarClipDist (f64 f_dst)
{ far_clip_dst = f_dst;
  return *this;
}

Bolex &Bolex::SetNearAndFarClipDist (f64 n_dst, f64 f_dst)
{ near_clip_dst = n_dst;
  far_clip_dst = f_dst;
  return *this;
}


}  // mortal coils and shuffling all 'round: especially you, namespace zeugma.
