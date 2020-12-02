
#include "SonoChoosist.h"

#include "LoopZoft.h"

#include "TrGrappler.h"

#include "vector_interop.hpp"


using namespace charm;


Choizl::Choizl ()  :  Node ()
{ TextureParticulars tepa
    = CreateTexture2D ("../ui-visuals/bordered-circle-tridot-256-alpha.png",
                       DefaultTextureFlags);
  texre = new TexturedRenderable (tepa);
  AppendRenderable (texre);
//  texre -> AdjColorZoft () . BecomeLike (InterpColor ());
  texre -> SetAdjColor (ZeColor (1.0, 0.15));
  Scale (mopey_sca);
  mopey_sca . SetHard (Vect (1.0));
  Rotate (ZoftVect (Vect::zaxis),
          LoopFloat (0.0, 2.0 * M_PI * (0.05 + 0.15 * drand48 ())));
  Translate (perky_loc);
}


#define ERPTI (Tamparams::Current ()->sono_choosist_furl_time)


SonoChoosist::SonoChoosist (const PlatonicMaes *maes)  :  Alignifer (),
                                                          behalf_of (NULL)
{ if (! maes)
    { wid = 200.0;  hei = 40.0;  brd_thc = 5.0; }
  else
    { wid = 0.9 * maes -> Width ();
      hei = 0.075 * wid;
      brd_thc = 0.5 * (maes -> Width ()  -  wid);
      AlignToMaes (maes);
    }

  AppendRenderable (brdr_re = new PolygonRenderable);
  brdr_re -> SetShouldFill (false);
  brdr_re -> SetShouldEdge (true);

  brdr_re -> AppendVertex (crn_lr);
  brdr_re -> AppendVertex (crn_ur);
  brdr_re -> AppendVertex (crn_ul);
  brdr_re -> AppendVertex (crn_ll);

  Vect hrz = 0.5 * wid * Vect::xaxis;
  Vect vrt = 0.5 * hei * Vect::yaxis;
  frl_lr = unf_lr = hrz - vrt;
  frl_ur = unf_ur = hrz + vrt;
  unf_ll = -hrz - vrt;
  unf_ul = -hrz + vrt;
  frl_ll = frl_lr - hei * Vect::xaxis;
  frl_ul = frl_ur - hei * Vect::xaxis;

  crn_lr . SetHard (frl_lr);
  crn_ur . SetHard (frl_ur);
  crn_ul . SetHard (frl_ul);
  crn_ll . SetHard (frl_ll);

  crn_lr . SetInterpTime (ERPTI);
  crn_ur . SetInterpTime (ERPTI);
  crn_ul . SetInterpTime (ERPTI);
  crn_ll . SetInterpTime (ERPTI);

  active . SetInterpTime (ERPTI);
active . SetHard (1.0);

  chz_dia = 0.9 * hei;
}


void SonoChoosist::SetNumChoizls (i64 nc)
{ if (nc  <  0)
    nc = 0;

  i64 curn = NumChoizls ();
  if (nc  == curn)
    return;

  if (nc  >  curn)
    { while (curn  <  nc)
        { Choizl *chz = new Choizl;
          choizls . push_back (chz);
          chz->mopey_sca .SetHard (Vect (chz_dia));
          AppendChild (chz);
if (curn == 0)      chz->texre->SetAdjColor(ZeColor(1.0,0.0,0.0,0.2));
else if (curn == 1) chz->texre->SetAdjColor(ZeColor(0.0,1.0,0.0,0.2));
else if (curn == 2) chz->texre->SetAdjColor(ZeColor(0.0,0.0,1.0,0.2));
else if (curn == 3) chz->texre->SetAdjColor(ZeColor(1.0,0.0,1.0,0.2));
else if (curn == 4) chz->texre->SetAdjColor(ZeColor(1.0,1.0,0.0,0.2));
          ++curn;
        }
    }
}


void SonoChoosist::InitiateAtomicContact (Ticato *tic)
{ if (! tic)
    return;
  if (behalf_of)
    { // some farewell gesture to whomever we'd been serving?
    }
  behalf_of = tic;
}

void SonoChoosist::Furl ()
{ crn_lr . Set (frl_lr);  crn_ur . Set (frl_ur);
  crn_ul . Set (frl_ul);  crn_ll . Set (frl_ll);

  i64 nc = NumChoizls ();
  if (nc  <  1)
    return;

  Vect p = 0.25 * (frl_ll + frl_ul + frl_ur + frl_lr);
  for (i64 q = 0  ;  q < nc  ;  ++q)
    { choizls[q]->perky_loc . Set (p); }
}


void SonoChoosist::Unfurl ()
{ crn_lr . Set (unf_lr);  crn_ur . Set (unf_ur);
  crn_ul . Set (unf_ul);  crn_ll . Set (unf_ll);

  i64 nc = NumChoizls ();
  if (nc  <  1)
    return;

  f64 spcng = wid - (f64)nc * chz_dia;
  spcng /= (f64)(nc + 1);
  Vect p = (0.5 * wid  - spcng - 0.5 * chz_dia) * Vect::xaxis;
  spcng += chz_dia;
  for (i64 q = 0  ;  q < nc  ;  ++q)
    { choizls[q]->perky_loc . Set (p);
      p -= spcng * Vect::xaxis;
    }
}


i64 SonoChoosist::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  if (! Active ())
    return 0;

  const std::string &prv = e -> Provenance ();
  const Vect &p = e -> Loc ();
  // a crap geometric job for now; just see if we're in the up-projected
  // parallelepiped...
  // f64 w = (crn_ur.val - crn_ul.val) . Dot (CurOver ());
  // f64 h = (crn_ur.val - crn_lr.val) . Dot (CurUp ());
// some sort of lesson. in the above, the crn are in local coords; ovr & upp not.
  f64 w = (crn_ur.val - crn_ul.val) . Dot (Vect::xaxis);
  f64 h = (crn_ur.val - crn_lr.val) . Dot (Vect::yaxis);
  Vect c = 0.25 * (crn_lr.val + crn_ur.val + crn_ul.val + crn_ll.val);
  Matrix44 m = from_glm (GetAbsoluteTransformation ().model);
  m . TransformVectInPlace (c);
  Vect hit;
  if (! G::RayRectIntersection (p, -CurNorm (),
                                c, CurOver (), CurUp (), w, h, &hit))
    return 0;
fprintf(stderr,"SMACKED old pal SONOCHOOSIST... with w/h = %.2lf/%2lf\n",w,h);

  if (! G::RayPlaneIntersection (p, -CurNorm (), c, CurNorm (), &hit))
    return 0;  // not actually possible, but we are thorough, yes. yes yes.

  f64 rsq = 0.25 * chz_dia * chz_dia;
  i64 ind = -1;
  for (Choizl *chz  :  choizls)
    { ++ind;
      if (! chz)
        continue;
      c = m . TransformVect (chz->perky_loc.val);
      if ((c - hit) . AutoDot ()  >  rsq)
        continue;
      f64 d = hit . DistFrom (p);
      if (d  <=  contact_dist)
        { if (smack . find (prv)  !=  smack . end ())
            return 1;  // already in contact
          auto it = hover . find (prv);
          if (it  !=  hover . end ())
            hover . erase (it);
          else  // we weren't hovering over this choizl but... here we are
            { }
          smack[prv] = chz;
          if (behalf_of)
            { if (ind == 0)
                behalf_of -> SonoSilence ();
              else
                behalf_of -> EnunciateNthSonoOption (ind - 1);
            }
          return 1;
        }
      else
        { auto it = smack . find (prv);
          if (it  !=  smack . end ())
            smack . erase (it);

          auto ht = hover . find (prv);
          if (ht  ==  hover . end ())
            { hover[prv] = chz;
              // and light us up
            }
          else if (ht->second  !=  chz)
            { ht->second = chz;
              // and also light up new hoveree, unlight old
            }
          else
            { // it's just the same dull choizl again. anything to do?
            }
          return 1;
        }
    }

  return 1;
}
