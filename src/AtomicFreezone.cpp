
#include "AtomicFreezone.h"

#include "ZEBulletinEvent.h"

#include "SinuZoft.h"
#include "ScGrappler.h"

#include "tamparams.h"

#include <GraphicsApplication.hpp>


AtomicFreezone::AtomicFreezone ()  :  SpectacleCauseway (),
                                      cineganz (NULL),
                                      atom_count_goal (45),
                                      inter_arrival_t (5.0),
                                      min_speed (75.0), max_speed (250.0),
                                      privileged_atom_probability (0.0)
{ // try auto-inhale
  // if (IronLung *irlu = IronLung::GlobalByName ("omni-lung"))
  //   irlu -> AppendBreathee (this);
}


AtomInfoPair AtomicFreezone::ClipFromUniqueAtomName (const std::string &uq_nm)
{ if (cineganz)
    for (const FilmInfo &fimm  :  *cineganz)
      if (const ClipInfo *clinp = fimm . ClipFromUniqueAtomName (uq_nm))
        return { &fimm, clinp };
  return { NULL, NULL };
}


bool AtomicFreezone::AppendPrivilegedAtom (const std::string &canon_name)
{ AtomInfoPair aip = ClipFromUniqueAtomName (canon_name);
  if (aip.first  !=  NULL)
    { privileged_atoms . push_back (aip);
      return true;
    }
  return false;
}


Ticato *AtomicFreezone::FirstHitAtom (const G::Ray &ra, Vect *hitp)
{ auto omega = atoms . rend ();
  for (auto it = atoms . rbegin ()  ;  it != omega  ;  ++it)
    if (Ticato *tic = *it)
      if (tic->fr  &&  tic->fr -> CheckHit (ra, hitp))
          return tic;
  return NULL;
}


void AtomicFreezone::DetachAndDisposeOfAtom (Ticato *icat)
{ if (! icat)
    return;
  auto it = std::find (atoms . begin (), atoms . end (), icat);
  if (it  ==  atoms . end ())
    { fprintf (stderr, "DisposeOfAtom() called with an atom-not-among-us...\n");
      return;
    }
  atoms . erase (it);
  if (Node *par = icat -> Parent ())
    par -> RemoveChild (icat);
  else
    delete icat;
}


Ticato *AtomicFreezone::InstanitateAtom (const Vect &loc, PlatonicMaes *mae,
                                         i32 direc)
{ Ticato *tic;
  if (drand48 ()  >=  privileged_atom_probability)
    tic = new Ticato (*cineganz);
  else
    { i64 ind = (i64)(drand48 () * (f64)privileged_atoms . size ());
      AtomInfoPair aip = privileged_atoms[ind];
      tic = new Ticato (*aip.first, *aip.second);
    }

  tic->born_from_flick = false;

  tic->sca . SetHard (400.0 + drand48 () * 300.0);
  tic->loc . SetHard (loc);
//tic->re->SetAdjColor (ZeColor (0.1, 1.0, 0.1, 1.0));
tic -> ProvisionVisibleBounds ();
  tic -> SetAndAlignToMaes (mae);
//tic -> BBoxSetColor (Tamglobals::Only ()->escatom_bbox_color);
tic -> BBoxSetColor (ZeColor (1.0, 0.0));

  f64 spd = min_speed + drand48 () * (max_speed - min_speed);
  spd *= (direc == 0)  ?  (drand48 () > 0.5 ? 1.0 : -1.0)  :  direc;
  tic->wander_vel . SetHard (Vect (spd, 0.0, 0.0));

  amok_field -> AppendChild (tic);
  atoms . push_back (tic);

  return tic;
}

/*
Ticato *AtomicFreezone::ReceiveAtomGraciously (Ticato *tic)
{ if (! tic)
    return NULL;

  f64 spd = min_speed + drand48 () * (max_speed - min_speed);
  spd *= (drand48 () > 0.5 ? 1.0 : -1.0);
  tic->wander_vel . SetHard (Vect (spd, 0.0, 0.0));

//  amok_field -> AppendChild (tic);
  if (std::find (atoms . begin (), atoms . end (), tic)  ==  atoms . end ())
    atoms . push_back (tic);
  return tic;
}
*/

void AtomicFreezone::SpontaneouslyGenerateAtomAtBoundary ()
{ i64 which_coast = drand48 () < 0.5  ?  0  :  1;
  Swath *sw = meander[which_coast * (meander . size () - 1)];
  Vect rone_p = which_coast  ?  sw->prone.pt2  :  sw->prone.pt1;
  f64 t = drand48 ();
  Vect lumb_p = sw->plumb.pt1  +  t * (sw->plumb.pt2 - sw->plumb.pt1);
  InstanitateAtom (rone_p + lumb_p - sw->prone.pt1, sw->supporting_maes,
                   which_coast > 0  ?  -1  :  1);
}


void AtomicFreezone::PopulateFromScratch ()
{ meander_len = MeanderLength (); //0.0;
  // for (Swath *sw  :  meander)
  //   meander_len += sw->prone . Length ();

  for (i64 q = (i64)atom_count_goal  ;  q > 0  ;  --q)
    { f64 linpos = meander_len * drand48 ();
      f64 ell;
      Ticato *tic = NULL;
      for (Swath *sw  :  meander)
        if (linpos  <  (ell = sw->prone . Length ()))
          { f64 t = linpos / ell;
            Vect rone_p = sw->prone.pt1  +  t * (sw->prone.pt2 - sw->prone.pt1);
            t = drand48 ();
            Vect lumb_p = sw->plumb.pt1  +  t * (sw->plumb.pt2 - sw->plumb.pt1);
            tic = InstanitateAtom (rone_p + lumb_p - sw->prone.pt1,
                                   sw->supporting_maes, 0);
            break;
          }
        else
          linpos -= ell;
    }
}


void AtomicFreezone::PerambulizeAtoms (f64 dt)
{ std::vector <Ticato *> *mort = NULL;
  for (Ticato *tic  :  atoms)
    if (tic)
      { if (! tic -> CurYanker () . empty ())
          continue;

        f64 vx = tic->wander_vel.val.x;
        const Vect &ovr = tic->cur_maes -> Over ();
        Vect newloc = tic->loc.val  +  dt * vx * ovr;

        Swath *curth = SwathFor (tic->cur_maes);
assert (curth != NULL);
        if (vx < 0.0  &&  (newloc - curth->prone.pt1) . Dot (ovr)  <  0.0)
          curth = curth->prv;
        else if (vx > 0.0  &&  (newloc - curth->prone.pt2) . Dot (ovr)  >  0.0)
          curth = curth->nxt;
        else
          { tic->loc . Set (newloc);
            continue;
          }

        // at this point, we've run off the end of our present maes
        if (curth)
          { Vect n = curth->supporting_maes -> Norm ();
            f64 t = (newloc - curth->supporting_maes->loc) . Dot (n);
            newloc -= t * n;  // get back on the plane, jack
            tic->loc . Set (newloc);
            tic -> SetAndAlignToMaes (curth->supporting_maes);
            continue;
          }
        // now, here, sadly, tic is homeless and must be raptured
        if (! mort)
          mort = new std::vector <Ticato *> ();
        mort -> push_back (tic);
      }

  if (mort)
    { for (Ticato *tic  :  *mort)
        DetachAndDisposeOfAtom (tic);
      delete mort;
    }
}


void AtomicFreezone::UndertakeLongRangeDimming ()
{ if (dim_rad.val  <  0.0)
    { for (Ticato *tic  :  atoms)
        if (tic)
          if (tic->dim_adj . PointB ().val  !=  1.0)
            tic->dim_adj . Set (1.0);
      return;
    }

  f64 rsq = dim_rad.val * dim_rad.val;
  for (Ticato *tic  :  atoms)
    if (tic)
      { Vect ax_prj = G::PointOntoLineProjection (tic->loc.val, dim_axis);
        if ((tic->loc.val - ax_prj) . AutoDot ()  <  rsq)
          { if (tic->dim_adj . PointB ().val  !=  0.0)
              tic->dim_adj . Set (0.0);
          }
        else
          { if (tic->dim_adj . PointB ().val  !=  1.0)
              tic->dim_adj . Set (1.0);
          }
      }
}


i64 AtomicFreezone::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;
  Vect hit (INITLESS);
  const std::string &prv = e -> Provenance ();
  if (Ticato *tic = AtomYankedBy (prv))
    { if (GraphicsApplication *g = GraphicsApplication::GetApplication ())
        { Vect hit (INITLESS);
          if (PlatonicMaes *maes
              = g -> ClosestIntersectedMaes (e -> Loc (), e -> Aim (), &hit))
            { tic -> LocZoft () . Set (hit + tic->gropoff);
              if (maes  !=  tic -> CurMaes ())
                { ZEBulletinEvent *bev
                    = new ZEBulletinEvent ("drag-maes-change");
                  bev -> AppendObjPhrase ("dragee", tic);
                  bev -> AppendObjPhrase ("from-maes", tic -> CurMaes ());
                  bev -> AppendObjPhrase ("to-maes", maes);
                  bev -> SetForebearEvent (e);
                  GraphicsApplication::GetApplication ()
                    -> GetSprinkler () . Spray (bev);
                  delete bev;
                  tic -> SetAndAlignToMaes (maes);
                }
            }
        }
    }
  else if (Ticato *tic = FirstHitAtom (G::Ray {e -> Loc (), e -> Aim ()}, &hit))
    { auto it = hoverees . find (prv);
      if (it  ==  hoverees . end ())
        { tic -> BeHoveredBy (prv);
          hoverees[prv] = tic;
          fprintf (stderr, "hovering athwart <%p>\n", tic);
        }
      else if (it->second  !=  tic)
        { it->second -> BeNotHoveredBy (prv);
          tic -> BeHoveredBy (prv);
          hoverees[prv] = tic;  // jumped from one to 'nother with naught between
        }
      else
        ;  // already hovering
    }
  else
    { auto it = hoverees . find (prv);
      if (it  !=  hoverees . end ())
        { it->second -> BeNotHoveredBy (prv);
          fprintf (stderr, "Lo: abandoning <%p>\n", it->second);
          hoverees . erase (it);
        }
    }
  return 0;
}


i64 AtomicFreezone::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (! e)
    return -1;
  const std::string &prv = e -> Provenance ();
  if (Ticato *tic = AtomHoveredBy (prv))
    { tic -> BeNotHoveredBy (prv);
      tic -> BeYankedBy (prv);
      yankees[prv] = tic;
      Vect hit (INITLESS);
      if (! tic->cur_maes
          ||  ! G::RayPlaneIntersection (e -> Loc (), e -> Aim (),
                                         tic->cur_maes -> Loc (),
                                         tic->cur_maes -> Norm (), &hit))
        assert (&"good" == &"evil");
      tic->gropoff . SetHard (tic -> CurLoc ()  -  hit);
      tic->gropoff . Set (Vect::zerov);
      auto it = hoverees . find (prv);
      if (it  !=  hoverees . end ())
        hoverees . erase (it);
      if (Node *conv = Tamglobals::Only ()->conveyor)
        { tic->from_node = tic -> Parent ();
          conv -> AppendChild (tic);
        }
      return 1;  // i.e. nobody else should see the harden, mkay?
    }
  else
    fprintf (stderr, "baseless CLICKsterism. here in AFreezo's SpHarden...\n");
  return 0;
}


i64 AtomicFreezone::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (! e)
    return -1;
  const std::string &prv = e -> Provenance ();
  if (Ticato *tic = AtomYankedBy (prv))
    { tic -> BeNotYankedBy (prv);
      // we dont' return to hovering because (among other things) we may
      // now be on a different surface with different hover semantics/mechanics
      if (tic->from_node)
        { tic->from_node -> AppendChild (tic);
          tic->from_node = NULL;
          // that is: put us back where we were in the scene graph, knowing
          // that the recipient of the bulletin event immediately following
          // may rip us away once more to locate us elsegraphular
        }
      auto it = yankees . find (prv);
assert (it != yankees . end ());
      yankees . erase (it);
      PlatonicMaes *onto_maes = tic -> CurMaes ();
      if (! SwathFor (onto_maes))
        { ZEBulletinEvent *bev = new ZEBulletinEvent ("atom-deposit");
          bev -> AppendObjPhrase ("inbound-atom", tic);
          bev -> AppendObjPhrase ("from-zone", this);
          bev -> AppendObjPhrase ("onto-maes", onto_maes);
          bev -> SetForebearEvent (e);
          GraphicsApplication::GetApplication ()
            -> GetSprinkler () . Spray (bev);
          delete bev;
          auto iitt = std::find (atoms . begin (), atoms . end (), tic);
assert (iitt  !=  atoms . end ());
          atoms . erase (iitt);
        }
    }
  else
    fprintf (stderr, "... and like that, CLICKY is gone from hovering.\n");
  return 0;
}


i64 AtomicFreezone::ZEBulletin (ZEBulletinEvent *e)
{ if (! e)
    return -1;
  const ZEBStrPhrase *phr;
  Ticato *tic;

  if (e -> Says ("atom-deposit"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("inbound-atom")))
        if (PlatonicMaes *emm
            = dynamic_cast <PlatonicMaes *> (e -> ObjByTag ("onto-maes")))
          if (std::find (underlying_maeses . begin (),
                         underlying_maeses . end (), emm)
              !=  underlying_maeses . end ())
            { tic->accom_sca . Set (Vect (1.0));
              tic->shov_vel = Vect::zerov;

              atoms . push_back (tic);
              amok_field -> AppendChild (tic);
              tic -> SetAndAlignToMaes (emm);

              f64 spd = min_speed  +  drand48 () * (max_speed - min_speed);
              spd *= (drand48 () < 0.5  ?  -1.0  :  1.0);
              tic->wander_vel . SetHard (Vect (spd, 0.0, 0.0));
              // tic -> BBoxSetColor (Tamglobals::Only ()->tabatom_bbox_color);

              if (phr = e -> FindStrPhrase ("plucked-from-flick-by"))
                { tic -> BeYankedBy (phr->str);  // that's the provenance
                  yankees[phr->str] = tic;
                  if (Node *conv = Tamglobals::Only ()->conveyor)
                    { //tic->from_node = tic -> Parent ();
                      conv -> AppendChild (tic);
                    }
                  // ReceiveAtomGraciously (tic);  // don' need no mo'
                }
            }
    }
  return 0;
}


i64 AtomicFreezone::Inhale (i64, f64 thyme)
{ float dt = thyme - prev_time;
  PerambulizeAtoms (dt);

  if (atoms . size ()  <  atom_count_goal)
    if (drand48 ()  <  dt / inter_arrival_t)
      SpontaneouslyGenerateAtomAtBoundary ();

  if (dim_rad.val  !=  0.0)
    UndertakeLongRangeDimming ();

  prev_time = thyme;
  return 0;
}
