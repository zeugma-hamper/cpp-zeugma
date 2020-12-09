
#include "Orksur.h"

#include "SinuZoft.h"
#include "SumZoft.h"

#include "ZeUUID.h"

#include "vector_utils.hpp"

#include "tamparams.h"

#include "GraphicsApplication.hpp"

#include <vector>


Orksur::Orksur (const PlatonicMaes &ma)  :  PlatonicMaes (ma, false),
                                            underlying_maes (&ma),
                                            collage (new Node),
                                            soncho (new SonoChoosist (&ma)),
                                            associated_wallmaes (NULL),
                                            sentient_dist (200.0),
                                            contact_dist (25.0)
{ AppendChild (collage);
  AppendChild (soncho);
  soncho -> SetNumChoizls (5);
  soncho->loc . Set (Loc ()  +  0.5 * (soncho->hei + 2.0 * soncho->brd_thc
                                       - ma . Height ()) * ma . Up ());
  soncho -> Furl ();
  soncho->contact_dist = this->contact_dist;
}


/*
Splort *Orksur::NewSplort (f64 rad, i64 num_verts)  const
{ Splort *spl = new Splort;

  for (i64 w = 0  ;  w < 2  ;  ++w)
    for (i64 q = 0  ;  q < num_verts  ;  ++q)
      { f64 theeeta = 2.0 * M_PI / (f64)num_verts * (f64)q  +  w * M_PI;
        Vect radial = cos (theeeta) * ovr  +  sin (theeeta) * upp;
        radial *= 0.5 * (w + 1.0);
        SinuVect arm (0.065 * rad * radial, 5.0 + 0.7 * drand48 (),
                      0.24 * (1.0 + 3.0 * (q%2)) * rad * radial);
        SumVect voitecks (spl->loc, arm);
        (w > 0 ? spl->re1 : spl->re2) -> AppendVertex (voitecks);
      }

  return spl;
}
*/


stringy_list Orksur::CollageAtomsNameList ()
{ stringy_list nlist;
  for (Ticato *tic  :  players)
    if (tic)
      nlist . push_back (tic -> AtomName ());
  return nlist;
}


void Orksur::AtomicFirstStrike (Ticato *tic)
{ if (! tic)
    return;
  if (soncho)
    soncho -> InitiateAtomicContact (tic);
}


Ticato *Orksur::ClosestAtom (const Vect &p)
{ Ticato *ct = NULL;
  f64 d, cd = CHRM_MAX_F64;

  for (Ticato *tic  :  players)
    if (tic)
      if ((d = tic -> CurLoc () . DistFrom (p))  <  cd)
        { ct = tic;  cd = d; }
  return ct;
}


void Orksur::DistinguishManipulees ()
{ static ZeColor fullc (1.0, 1.0);
  static ZeColor fadec (0.4, 1.0);

  bool never_mind = (hoverees . size ()  ==  0);
  for (Ticato *tic  :  players)
    if (tic)
      tic->re -> SetAdjColor (never_mind ? fullc : fullc); //fadec);

  for (auto &f  :  graspees)
    if (f.second.tic)
      f.second.tic->re -> SetAdjColor (ZeColor (2.0, 1.0));
}


bool Orksur::AppendAtomToCollage (Ticato *tic)
{ if (! tic)
    return false;

  auto it = std::find (players . begin (), players . end (), tic);
  if (it  !=  players . end ())
    { // again, something plentifully wrong; why's it already here?
      assert (2 == 3);
    }

  stringy_list extant_atoms = CollageAtomsNameList ();

  collage -> AppendChild (tic);  // excises from former parent, see?
  players . push_back (tic);
  tic->accom_sca
    . Set (tic -> BornViaFlickPluck ()
           ?  Vect (Tamparams::Current ()->table_flmatom_scale_factor)
           :  Vect (Tamparams::Current ()->table_escatom_scale_factor));

  if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
    { sherm -> SendPlayBoop (5);
      i64 disc_id = ZeMonotonicID ();
      awaiting_audio_sooth[disc_id] = tic;
      sherm -> SendGetSuggestions
        (extant_atoms, tic -> AtomName (),
         (tic->atom_info ? tic->atom_info->duration : 0.0),
         disc_id);
    }

  return true;
}


bool Orksur::RemoveAtomFromCollage (Ticato *tic)
{ if (! tic)
    return false;
  FindErase (players, tic);
  tic -> Parent () -> RemoveChild (tic);
  return true;
}


void Orksur::SilenceAllAtoms ()
{ for (Ticato *tic  :  players)
    tic -> SonoSilence ();
}


AtomThusness Orksur::ImpelFreeAtom (Ticato *tic, f64 dt)
{ if (! tic)
    return AtomThusness::CONT;

  Vect newp = tic -> CurLoc ()  +  dt * tic->shov_vel;
  if (G::PointRectContainment (newp, Loc (), Over (), Up (),
                               Width (), Height (), NULL))
    { tic -> LocZoft () . SetHard (newp);
      return AtomThusness::CONT;
    }

  // we're off the edge of the table; project to wall.
  Vect hit;
  if (! G::RayPlaneIntersection (tic -> CurLoc (), tic->shov_vel,
                                 associated_wallmaes -> Loc (),
                                 associated_wallmaes -> Norm (), &hit))
    { // off the table in a direction that doesn't hit a wall
      return AtomThusness::NUKE;
    }

  tic -> LocZoft () . SetHard (hit);
  auto it = std::find (players . begin (), players . end (), tic);
  if (it  ==  players . end ())
    { // well, that's if not impossible then certaianly horrible.
      assert ('a' == 'z');
    }
  players . erase (it);

  ZEBulletinEvent *bev = new ZEBulletinEvent ("atom-deposit");
  bev -> AppendObjPhrase ("inbound-atom", tic);
  bev -> AppendObjPhrase ("from-zone", this);
  bev -> AppendObjPhrase ("onto-maes", associated_wallmaes);
  GraphicsApplication::GetApplication () -> GetSprinkler () . Spray (bev);
  delete bev;

  return AtomThusness::CONT;
}


void Orksur::DisposeOfCollage ()
{ Vect o = Over ();
  Vect u = Up ();
  f64 halfang;
  Vect hit;
  if (associated_wallmaes
      &&  G::RayPlaneIntersection (Loc (), u, associated_wallmaes -> Loc (),
                                   associated_wallmaes -> Norm (), &hit))
    halfang = atan2 (0.9 * 0.5 * associated_wallmaes -> Width (),
                     Loc () . DistFrom (hit));
  else
    halfang = M_PI / 180.0 * 30.0;
  for (Ticato *tic  :  players)
    { f64 ang = 2.0 * (drand48 () - 0.5) * halfang;
      f64 spd = Tamparams::Current ()->disposal_speed_threshold
         * (1.0 + drand48 ());
      tic->shov_vel = spd * (cos (ang) * u  +  sin (ang) * o);
    }
}

void Orksur::EliminateCollage ()
{ for (Ticato *tic  :  players)
    { if (Node *par = tic -> Parent ())
        par -> RemoveChild (tic);
      else
        delete tic;
    }
  players . clear ();
  hoverees . clear ();
  graspees . clear ();
}



void Orksur::WhisperSplortily (const std::string &prv, const Vect &proj, f64 dst)
{ Splort *s;
  try  { s = splorts . at (prv); }
  catch (const std::out_of_range &e)
    { AppendChild (s = new Splort);
      splorts[prv] = s;
    }
  s->loc . Set (proj);

  Vect bloff = proj - CornerBL ();
  Vect troff = CornerTR () - proj;
  s->re -> ClearAllLines ();

  const Vect &o = ovr.val, &u = upp.val;
  f64 l = o . Dot (bloff), r = o . Dot (troff);
  if (l < 0.0  ||  r < 0.0)
    return;
  f64 b = u . Dot (bloff), t = u . Dot (troff);
  if (b < 0.0  ||  t < 0.0)
    return;

  s->re -> AppendLine (proj - l * o, proj + r * o);
  s->re -> AppendLine (proj - b * u, proj + t * u);
  float aa = (dst - contact_dist) / (sentient_dist - contact_dist);
  if (aa < 0.0)
    aa = 0.0;
  else if (aa > 1.0)
    aa = 1.0;
  s->re -> SetLinesColor (ZeColor (1.0, 0.5 * (1.0 - aa)));
}


i64 Orksur::ConfectSpatialPointingFromPosition (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  const std::string &prv = e -> Provenance ();
  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 t = n . Dot (p - loc);
  Vect proj = p  -  t * n;

  // ensure that we're within the airspace of the surface.
  // note: may want to restrict this check to harden...
  f64 w = 0.5 * wid.val;
  f64 h = 0.5 * hei.val;
  Vect locl = proj - loc.val;
  f64 u = locl . Dot (ovr.val);
  if (u < -w  ||  u > w)
    return 0;
  if ((u = locl . Dot (upp . val))  <  -h  ||  u > h)
    return 0;

  if (prev_prox_by_prov . find (prv)  ==  prev_prox_by_prov . end ())
    prev_prox_by_prov[prv] = 2.0 * sentient_dist;
  f64 pprox = prev_prox_by_prov[prv];

  if (t >  sentient_dist)
    { // perhaps start a timer whose runout triggers de-hovering etc.?
      if (pprox  <=  sentient_dist)
        {  // spatial-vanish event
        }
      prev_prox_by_prov[prv] = t;
      return 0;
    }

  static std::string syn_s = "synth-";
  std::string newprv = syn_s + prv;

  if (pprox  >  sentient_dist)
    {  // spatial-appear event
    }

  // no matter what at this point we needs must emit a move event
  ZESpatialMoveEvent zsme (newprv, e -> Loc (), -Norm (), Over ());
  zsme . SetForebearEvent (e);
  // now the big question: do we route through general event distribution,
  // or just send to ourselves? maybe the latter for now?
  ZESpatialMove (&zsme);

  if (t  <=  contact_dist)
    { if (pprox  >  contact_dist)  // this here's the harden case
        { ZESpatialHardenEvent zshe (zsme, 0);
          zshe . SetForebearEvent (e);
          ZESpatialHarden (&zshe);
        }
    }
  else  // that is: we're hover -- between harden and oblivion
    { if (pprox  <=  contact_dist)  // soften, wouldn't you know...
        { ZESpatialSoftenEvent zsse (zsme, 0);
          zsse . SetForebearEvent (e);
          ZESpatialSoften (&zsse);
        }
    }
  prev_prox_by_prov[prv] = t;
  return 1;
}


i64 Orksur::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  if (e -> Aim () . Norm () . Dot (Norm ())  >  -0.99)
    return ConfectSpatialPointingFromPosition (e);

  const std::string &prv = e -> Provenance ();

  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 tt = n . Dot (p - loc);
  Vect proj = p  -  tt * n;
//  tt = fabs (tt);

  // constrain to some reasonable laterally-bounded airspace o'er the table
  if (! G::PointRectContainment (proj, Loc (), Over (), Up (),
                                 1.1 * Width (), 1.1 * Height ()))
    return 0;

  WhisperSplortily (prv, proj, tt);

  auto heff = hoverees . find (prv);
  auto geff = graspees . find (prv);
assert (! (heff != hoverees . end ()  &&  geff != graspees . end ()));

  // we'll give the sonochoosist first dibs
  if (soncho)
    if (soncho -> ZESpatialMove (e)  >  0)
      return 1;

  if (geff  !=  graspees . end ())  // we're dragging some atom around
    { Vect newp = proj + geff->second.gropoff;
      f64 dt = GraphicsApplication::GetFrameTime () -> GetCurrentDelta ();
      geff->second.tic->shov_vel
= Vect::zerov;
      geff->second.tic -> LocZoft () . Set (newp);
      return 1;
    }

  Ticato *ca = ClosestAtom (proj);
  if (heff  ==  hoverees . end ())
    { if (ca)
        hoverees[prv] = { ca, Vect::zerov };
    }
  else if (heff->second.tic  !=  ca)
    { // lighting change for outgoing?
      heff->second.tic = ca;
    }

//  DistinguishManipulees ();
  return 0;
}


i64 Orksur::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (! e)
    return -1;

  // let's reject if not over our airspace
  Vect proj;
  if (! G::PointRectContainment (e -> Loc (), Loc (), Over (), Up (),
                                 Width (), Height (), &proj))
    return 0;

  const std::string &prv = e -> Provenance ();
  auto heff = hoverees . find (prv);
  if (heff  ==  hoverees . end ())
    return 0;  // anythig else to do besides, you know, nothing?
/*
  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 tt = n . Dot (p - loc);
  Vect proj = p  -  tt * n;
*/
  Ticato *tic = heff->second.tic;
  hoverees . erase (heff);

  if (graspees . find (prv)  !=  graspees . end ())
    {  // it's shouldn't be possible... and yet... well, let's figure it out:
      assert (true == false);
    }

  graspees[prv] = { tic, (tic -> CurLoc () - proj) };
  tic -> MakeRenderablesForemostInLayer ();
  tic->shov_vel = Vect::zerov;
  tic->interp_adjc . Set (ZeColor (2.0, 1.0));
  AtomicFirstStrike (tic);
  // and also... light the lucky atom up? or like that?
  return 1;
}


i64 Orksur::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (! e)
    return -1;

  const std::string &prv = e -> Provenance ();
  auto geff = graspees . find (prv);
  if (geff  ==  graspees . end ())
    return 0;  // really just that simple?

  if (hoverees . find (prv)  !=  hoverees . end ())
    {  // the heck?
      assert (0.1  ==  1.0);
    }

  hoverees[prv] = geff->second;
  graspees . erase (geff);
  geff->second.tic->interp_adjc . Set (ZeColor (1.0, 1.0));
/*
  if (ca->shov_vel . Mag ()
      >=  Tamparams::Current ()->disposal_speed_threshold)
    { fprintf (stderr, "PREYZDE LAWD -- <%p> gon' scoot to die!", ca);
      // and but then as well... what.
// QQQ ca->shov_vel = Vect::zerov;
    }
*/
  return 0;
}


i64 Orksur::ZEYowlAppear (ZEYowlAppearEvent *e)
{ if (! e)
    return -1;
  const std::string &utt = e -> Utterance ();
  const char *ustr = utt . c_str ();
  if (*ustr  ==  '\0')
    return 0;

  char uch = *ustr;
  if (uch >= '0'  &&  uch <= '9')
    { i64 ind = uch - '0';
      if (ind  <  players . size ())
        sel_atom = players[ind];
    }
  else if (uch  ==  'Z')
    { if (sel_atom)
        sel_atom -> SonoSilence ();
    }
  else if (uch >= 'A'  &&  uch <= 'J')
    { i64 ind = uch - 'A';
      if (sel_atom)
        sel_atom -> EnunciateNthSonoOption (ind);
    }
  else if (utt  ==  "[")
    { if (soncho)  soncho -> Furl (); }
  else if (utt  ==  "]")
    { if (soncho)  soncho -> Unfurl (); }
  else if (utt  ==  "]")
    { if (soncho)  soncho -> Unfurl (); }
  else if (utt  ==  "/")
    { DisposeOfCollage (); }
  else if (utt  ==  "?")
    { EliminateCollage (); }
  else if (utt  ==  ".")
    { SilenceAllAtoms (); }
  return 0;
}

i64 Orksur::ZEBulletin (ZEBulletinEvent *e)
{ if (! e)
    return -1;
  ZEBObjPhrase *phr;
  Ticato *tic;

  if (e -> Says ("drag-maes-change"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("dragee")))
        if (e -> ObjByTag ("to-maes")  ==  underlying_maes)
          { auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  ==  inchoates . end ())
              { inchoates . push_back (tic);
                tic->accom_sca . Set
                  (tic -> BornViaFlickPluck ()
                   ?  Vect (Tamparams::Current ()->table_flmatom_scale_factor)
                   :  Vect (Tamparams::Current ()->table_escatom_scale_factor));
              }
          }
        else if (e -> ObjByTag ("from-maes")  ==  underlying_maes)
          { auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  !=  inchoates . end ())
              { inchoates . erase (it);
                tic->accom_sca . Set (Vect (1.0));
              }
          }
    }
  else if (e -> Says ("atom-deposit"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("inbound-atom")))
        if (e -> ObjByTag ("onto-maes")  ==  underlying_maes)
          { AppendAtomToCollage (tic);
            tic->wander_vel = Vect::zerov;
            //tic -> BBoxSetColor (Tamglobals::Only ()->tabatom_bbox_color);
            auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  !=  inchoates . end ())
              inchoates . erase (it);
          }
    }
  return 0;
}


i64 Orksur::TASSuggestion (TASSuggestionEvent *e)
{ if (! e)
    return -1;

  i64 disc_id = e -> GetDiscussionID ();
  auto awaiter = awaiting_audio_sooth . find (disc_id);
  if (awaiter  ==  awaiting_audio_sooth . end ())
    return 0;
  Ticato *tic = awaiter->second;
  awaiting_audio_sooth . erase (awaiter);

  stringy_list suggs = e -> GetSuggestionNames ();
fprintf (stderr, "WEEEEELLLLLP. Just got %d audio suggestions:\n",suggs.size());
for (auto &ess  :  suggs)
fprintf(stderr,"\"%s\", ", ess.c_str ());
fprintf(stderr,"\n");
  if (! tic)
    return -1;
  tic->sono_options = suggs;
// and presumably quite a bit more action...
  // if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
  //   if (suggs . size ()  >  0)
  //     { i32 ind = drand48 () * suggs . size ();
  //       fprintf(stderr,"about to request %d = [%s]...\n",ind,suggs[ind].c_str());
  //       sherm -> SendPlaySound (suggs . at (ind));
  //     }
  return 0;
}



i64 Orksur::Inhale (i64 ratch, f64 thyme)
{ std::vector <Ticato *> *mort = NULL;
  f64 dt = GraphicsApplication::GetFrameTime () -> GetCurrentDelta ();
  for (Ticato *tic  :  players)
    if (tic->shov_vel . AutoDot ()  >  0.0
        &&  ! AtomIsGrasped (tic))
      if (ImpelFreeAtom (tic, dt)  ==  AtomThusness::NUKE)
        { if (! mort)
            mort = new std::vector <Ticato *> ();
          mort -> push_back (tic);
        }

  if (mort)
    { for (Ticato *mortic  :  *mort)
        { auto it = std::find (players . begin (), players . end (), mortic);
          if (it  ==  players . end ())
            {  // whuh?
              assert (M_PI == 3.0);
            }
          players . erase (it);
        }
      delete mort;
    }

  return 0;
}
