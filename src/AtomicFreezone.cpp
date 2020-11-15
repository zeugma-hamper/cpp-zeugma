
#include "AtomicFreezone.h"


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


Ticato *AtomicFreezone::InstanitateAtom (const Vect &loc, PlatonicMaes *mae,
                                         i32 direc)
{ Ticato *tic = new Ticato (*cineganz);
  tic->cur_maes = mae;
  tic -> AlignToMaes ();
  tic->sca . SetHard (300.0);
  tic->loc . SetHard (loc);
  f64 spd = min_speed + drand48 () * (max_speed - min_speed);
  spd *= (direc == 0)  ?  (drand48 () > 0.5 ? 1.0 : -1.0)  :  direc;
  tic->vel . SetHard (Vect (spd, 0.0, 0.0));
  field_amok -> AppendChild (tic->no);
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
      { f64 vx = tic->vel.val.x;
        const Vect &ovr = tic->cur_maes -> Over ();
        Vect newloc = tic->loc.val  +  dt * vx * ovr;

        Swath *curth = SwathFor (tic->cur_maes);
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
        if (Node *par = tic->no -> Parent ())
          { par -> RemoveChild (tic->no);
            tic->no = NULL;
            tic->re = NULL;
          }
      }
  // disgusting. here it comes:
  if (mort)
    { auto pr = [mort] (Ticato *ti) -> bool
        { return std::find (mort->begin (), mort->end (), ti) != mort->end (); };
      atoms . erase (std::remove_if (atoms.begin (), atoms.end (), pr),
                     atoms.end ());
      for (Ticato *tic  :  *mort)
        delete tic;
      delete mort;
    }
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
