
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
            tic = new Ticato (*cineganz);
            tic->cur_maes = sw->supporting_maes;
            tic -> AlignToMaes ();
            tic->sca . Set (300.0);
            tic->loc . Set (rone_p + lumb_p - sw->prone.pt1);
            f64 spd = (drand48 () > 0.5 ? 1.0 : -1.0)
              * (min_speed + drand48 () * (max_speed - min_speed));
            tic->vel . Set (Vect (spd, 0.0, 0.0));
            field_amok -> AppendChild (tic->no);
            atoms . push_back (tic);
            break;
          }
        else
          linpos -= ell;
    }
}


void AtomicFreezone::PerambulizeAtoms (f64 dt)
{ for (auto it = atoms . begin ()  ;  it != atoms . end ()  ;  ++it)
    if (Ticato *tic = *it)
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

        // by this time, we've run off the end of our former maes
        if (curth)
          { Vect n = curth->supporting_maes -> Norm ();
            f64 t = (newloc - curth->supporting_maes->loc) . Dot (n);
            newloc -= t * n;
            tic->loc . Set (newloc);
            tic -> SetAndAlignToMaes (curth->supporting_maes);
            continue;
          }
        // now, here, sadly, tic is homeless and must be raptured
        if (Node *par = tic->no -> Parent ())
          { par -> ExciseChild (tic->no);
            tic->no = NULL;
            tic->re = NULL;
          }
        atoms . erase (it);
        // delete tic;
      }
}


i64 AtomicFreezone::Inhale (i64, f64 thyme)
{ PerambulizeAtoms (thyme - prev_time);

  prev_time = thyme;
  return 0;
}
