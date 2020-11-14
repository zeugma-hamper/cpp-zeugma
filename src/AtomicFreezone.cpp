
#include "AtomicFreezone.h"


void AtomicFreezone::PopulateFromScratch ()
{ meander_len = 0.0;
  for (auto &sw  :  meander)
    meander_len += sw.prone . Length ();

  for (i64 q = (i64)atom_count_goal  ;  q > 0  ;  --q)
    { f64 linpos = meander_len * drand48 ();
      f64 ell;
      Ticato *tic = NULL;
      for (auto &sw  :  meander)
        if (linpos  <  (ell = sw.prone . Length ()))
          { f64 t = linpos / ell;
            Vect rone_p = sw.prone.pt1  +  t * (sw.prone.pt2 - sw.prone.pt1);
            t = drand48 ();
            Vect lumb_p = sw.plumb.pt1  +  t * (sw.plumb.pt2 - sw.plumb.pt1);
            tic = new Ticato (*cineganz);
            tic->cur_maes = sw.supporting_maes;
            tic -> AlignToMaes ();
            tic->sca . Set (300.0);
            tic->loc . Set (rone_p + lumb_p - sw.prone.pt1);
            field_amok -> AppendChild (tic->no);
            atoms . push_back (tic);
          }
        else
          linpos -= ell;
    }
}


i64 AtomicFreezone::Inhale (i64, f64)
{ return 0; }
