
#include "AtomicFreezone.h"

#include "ZEBulletinEvent.h"

#include "SinuZoft.h"
#include "ScGrappler.h"

#include <GraphicsApplication.hpp>


void AtomicFreezone::AppendSwath (Swath *sw)
{ if (! sw)
    return;
  if (Swath *prev_sw
        = meander . size () > 0  ?  meander[meander . size () - 1] : NULL)
    { prev_sw -> nxt = sw;
      sw -> prv = prev_sw;
    }
  meander . push_back (sw);
}

Swath *AtomicFreezone::SwathFor (PlatonicMaes *ma)
{ for (Swath *sw  :  meander)
    if (sw  &&  ma == sw->supporting_maes)
      return sw;
  return NULL;
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
{ Ticato *tic = new Ticato (*cineganz);
  tic->cur_maes = mae;
  tic -> AlignToMaes ();
  tic->sca . SetHard (400.0 + drand48 () * 300.0);
  tic->loc . SetHard (loc);
//tic->re->SetAdjColor (ZeColor (0.1, 1.0, 0.1, 1.0));

  f64 spd = min_speed + drand48 () * (max_speed - min_speed);
  spd *= (direc == 0)  ?  (drand48 () > 0.5 ? 1.0 : -1.0)  :  direc;
  tic->vel . SetHard (Vect (spd, 0.0, 0.0));

  field_amok -> AppendChild (tic);
  atoms . push_back (tic);

  return tic;
}


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
{ meander_len = 0.0;
  for (Swath *sw  :  meander)
    meander_len += sw->prone . Length ();

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

        f64 vx = tic->vel.val.x;
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
fprintf(stderr,"WHACKING! WHACKING, I TELL YOU!\n");
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
            { tic -> LocZoft () . Set (hit);
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
          fprintf (stderr, "Lo! abandoning <%p>\n", it->second);
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
      auto it = hoverees . find (prv);
      if (it  !=  hoverees . end ())
        hoverees . erase (it);
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
      // should we immediately return to hovering? a hard one... but no.
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


i64 AtomicFreezone::Inhale (i64, f64 thyme)
{ float dt = thyme - prev_time;
  PerambulizeAtoms (dt);

  if (atoms . size ()  <  atom_count_goal)
    if (drand48 ()  <  dt / inter_arrival_t)
      SpontaneouslyGenerateAtomAtBoundary ();

  prev_time = thyme;
  return 0;
}
