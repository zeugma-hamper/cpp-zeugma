
#include "SonoChoosist.h"

#include "LoopZoft.h"
#include "SinuZoft.h"
#include "ScaleZoft.h"

#include "TrGrappler.h"

#include "vector_interop.hpp"


using namespace charm;


Choizl::Choizl ()  :  Node (), index (-1)
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
  active . SetHard (0.0);

  chz_dia = 0.9 * hei;

  AppendChild (chz_node = new Node);

  hexajig = new Jigglegon;
#define BRP 6
  hexajig -> SetNumVertices (BRP);
  for (i64 q = 0  ;  q < BRP  ;  ++q)
    hexajig -> NthVertex (q) . Set
      (0.6 * chz_dia * (cos (2.0 * M_PI / (f64)BRP * (f64)q) * Vect::xaxis
                        + sin (2.0 * M_PI / (f64)BRP * (f64)q) * Vect::yaxis));
  hexajig -> Populate (6, chz_dia * Vect (0.1, 0.1, 0.0));

  AppendChild (hexajig);

  oclo_no = new Node;
  oclo_re = new PolygonRenderable;
  oclo_no -> AppendRenderable (oclo_re);

  f64 lmax = 0.5 * chz_dia;
  f64 l0 = 0.33 * lmax;
  f64 lmin = 0.5 * l0;
  f64 ldif = 0.5 * (lmax - lmin);
  f64 lcnt = 0.5 * (lmax + lmin);
  SinuFloat sf_a (ldif, 0.15 + 0.1 * drand48 (), lcnt);
  SinuFloat sf_b (ldif, 0.15 + 0.1 * drand48 (), lcnt);

  for (i64 q = 0  ;  q < 16  ;  ++q)
    { f64 thayter = (f64)q * M_PI / 8.0;
      Vect r = cos (thayter) * Vect::xaxis  +  sin (thayter) * Vect::yaxis;
      if (q % 2)
        oclo_re -> AppendVertex (l0 * r);
      else if ((q / 2) % 2)
        oclo_re -> AppendVertex (ScaleVect (ZoftVect (r), sf_a));
      else
        oclo_re -> AppendVertex (ScaleVect (ZoftVect (r), sf_b));
    }
//  o_loc . SetInterpTime ();
  o_loc . SetHard (0.25 * (frl_ll + frl_ul + frl_ur + frl_lr));
  oclo_no -> Translate (o_loc);
  o_iro . SetHard (ZeColor (1.0, 0.0));
  oclo_re -> AdjColorZoft () . BecomeLike (o_iro);

  oclo_re -> SetShouldEdge (true);
  oclo_re -> SetEdgeColor (ZeColor (1.0, 0.25));
  oclo_re -> SetFillColor (ZeColor (1.0, 0.05));
  AppendChild (oclo_no);
}


void SonoChoosist::PopulateChoizls (i64 nc)
{ if (nc  <  0)
    nc = 0;

  nc++;  // yeah, high cheese: an invisible one to 'guide' the op/clo button...
  i64 curn = NumChoizls ();
  if (nc  == curn)
    return;

  Choizl *zeroch = NULL;

  if (nc  >  curn)
    { while (curn  <  nc)
        { Choizl *chz = new Choizl;
          choizls . push_back (chz);
          chz->mopey_sca . SetHard (Vect (chz_dia));
          chz_node -> AppendChild (chz);
if      (curn == 0) chz->texre->SetAdjColor(ZeColor(1.0,0.0));
else if (curn == 1) chz->texre->SetAdjColor(ZeColor(1.0,0.0,0.0,0.2));
else if (curn == 2) chz->texre->SetAdjColor(ZeColor(0.0,1.0,0.0,0.2));
else if (curn == 3) chz->texre->SetAdjColor(ZeColor(0.0,0.0,1.0,0.2));
else if (curn == 4) chz->texre->SetAdjColor(ZeColor(1.0,0.0,1.0,0.2));
else if (curn == 5) chz->texre->SetAdjColor(ZeColor(1.0,1.0,0.0,0.2));

          if (! zeroch)
            { zeroch = chz;
              oclo_no -> ClearTransforms ();
              oclo_no -> Translate (zeroch->perky_loc);
            }
          ++curn;
        }
    }
}


void SonoChoosist::MoveHexajigToChoizlN (i64 ind)
{ if (ind >= 0  &&  ind  <  choizls . size ())
    if (ZoftVect *zv = hexajig -> LocGrapplerZoftVect ())
      zv -> BecomeLike (choizls[ind]->perky_loc);
    else
      hexajig -> LocZoft () . Set (choizls[ind]->perky_loc.val);
}


void SonoChoosist::InitiateAtomicContact (Ticato *tic)
{ // if (! tic)
  //   return;
  if (behalf_of)
    { // some farewell gesture to whomever we'd been serving?
    }
  behalf_of = tic;
  if (! tic)
    { MoveHexajigToChoizlN (2);  // silence; but let's abstract, okay, Jim?
      hexajig -> RenderablesSetShouldNotDraw ();
      return;
    }
  i64 ind = 2 + tic->playing_sono;
  MoveHexajigToChoizlN (ind);

  if (Active ())
    hexajig -> RenderablesSetShouldDraw ();  // unnecessary unless 'first time'
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
  o_loc . Set (p);
  o_iro . Set (ZeColor (1.0, 0.0));

  hexajig -> RenderablesSetShouldNotDraw ();
  active . Set (0.0);
}


void SonoChoosist::Unfurl ()
{ crn_lr . Set (unf_lr);  crn_ur . Set (unf_ur);
  crn_ul . Set (unf_ul);  crn_ll . Set (unf_ll);

  i64 nc = NumChoizls ();
  if (nc  <  1)
    return;

  f64 spcng = wid - (f64)nc * chz_dia;
  spcng /= (f64)(nc + 1);
  Vect p = (0.5 * wid - spcng - 0.5 * chz_dia) * Vect::xaxis;
  spcng += chz_dia;
  for (i64 q = 0  ;  q < nc  ;  ++q)
    { // if (q > 0)
        choizls[q]->perky_loc . Set (p);
      // else
      //   o_loc . Set (p);
      p -= spcng * Vect::xaxis;
    }
  o_iro . Set (ZeColor (1.0, 1.0));

  if (behalf_of)
    { hexajig -> RenderablesSetShouldDraw ();
      MoveHexajigToChoizlN (2 + behalf_of->playing_sono);
    }
  active . Set (1.0);
}


bool SonoChoosist::PointInAirspaceOver (const Vect &p,
                                        Vect *hit_out, Matrix44 *mat_out)
{ // a crap geometric job for now; just see if we're in the up-projected
  // parallelepiped...
  // f64 w = (crn_ur.val - crn_ul.val) . Dot (CurOver ());
  // f64 h = (crn_ur.val - crn_lr.val) . Dot (CurUp ());
// some sort of lesson. in the above, the crn are in local coords; ovr & upp not.
  f64 w = (crn_ur.val - crn_ul.val) . Dot (Vect::xaxis);
  f64 h = (crn_ur.val - crn_lr.val) . Dot (Vect::yaxis);
  w += h;
  h += h;

  Vect c = Centerdom ();
  Matrix44 m = from_glm (GetAbsoluteTransformation ().model);
  m . TransformVectInPlace (c);
  Vect hit;
  if (! G::RayRectIntersection (p, -CurNorm (),
                                c, CurOver (), CurUp (), w, h, &hit))
    return false;
fprintf(stderr,"SMACKED old pal SONOCHOOSIST... with w/h = %.2lf/%2lf\n",w,h);

  if (! G::RayPlaneIntersection (p, -CurNorm (), c, CurNorm (), &hit))
    return false;  // not actually possible, but we are thorough, yes. yes yes.

  if (hit_out)
    *hit_out = hit;
  if (mat_out)
    *mat_out = m;
  return true;
}


i64 SonoChoosist::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  if (! Active ())
    return 0;

  const std::string &prv = e -> Provenance ();
  const Vect &p = e -> Loc ();
  Vect hit (INITLESS);
  Matrix44 m (INITLESS);
  if (! PointInAirspaceOver (p, &hit, &m))
    return 0;

  f64 rsq = 0.25 * chz_dia * chz_dia;
  i64 ind = -1;

  for (Choizl *chz  :  choizls)
    { ++ind;
      if (! chz)
        continue;
      Vect c = m . TransformVect (chz->perky_loc.val);
      if ((c - hit) . AutoDot ()  >  rsq)
        continue;

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

  return 0;
}


i64 SonoChoosist::ZESpatialHarden (ZESpatialHardenEvent *e)
{ const std::string &prv = e -> Provenance ();

  if (! PointInAirspaceOver (e -> Loc ()))
    return 0;

  if (! Active ())
    { Unfurl ();
      return 1;
    }

  if (smack . find (prv)  !=  smack . end ())
    return 1;  // already in contact
  auto it = hover . find (prv);
  if (it  !=  hover . end ())
    hover . erase (it);
  else  // weren't hovering over a choizl prior to contact
    return 0;

  Choizl *chz = it->second;
  auto cit = std::find (choizls . begin (), choizls . end (), chz);
  if (cit  ==  choizls . end ())  // nonsense! now then:
    assert (&"poop" == &"gold");

  i64 ind = cit - choizls . begin ();
  smack[prv] = chz;
  if (ZoftVect *zv = hexajig -> LocGrapplerZoftVect ())
    zv -> BecomeLike (chz->perky_loc);

  if (ind == 0)
    Furl ();
  else if (behalf_of)
    { if (ind == 1)
        behalf_of -> SonoSilence ();
      else
        behalf_of -> EnunciateNthSonoOption (ind - 2);
      behalf_of -> FlashAura ();
    }

  return 1;
}


i64 SonoChoosist::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ const std::string &prv = e -> Provenance ();

  // in this case we won't do the airspace test, as we might have been in
  // legitimate contact with a choizl but then slipped out of the airspace...
  auto it = smack . find (prv);
  if (it  ==  smack . end ())
    return 0;

  smack . erase (it);
  Choizl *chz = it->second;
  // Lowlight (chz);
  return 1;
}
