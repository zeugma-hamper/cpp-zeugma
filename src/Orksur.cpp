
#include "Orksur.h"

#include "SinuZoft.h"
#include "SumZoft.h"


Orksur::Orksur (const PlatonicMaes &ma)  :  PlatonicMaes (ma, false),
                                            underlying_maes (&ma)
{ }


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


i64 Orksur::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  const std::string &prv = e -> Provenance ();
  Splort *s;
  try  { s = splorts . at (prv); }
  catch (const std::out_of_range &e)
    { AppendChild (s = NewSplort (20.0));
      splorts[prv] = s;
    }

  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 t = n . Dot (p - loc);
  Vect proj = p  -  t * n;
  s->loc . Set (proj);

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
            if (it == inchoates . end ())
              inchoates . push_back (tic);
          }
        else if (e -> ObjByTag ("from-maes")  ==  underlying_maes)
          { auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  != inchoates . end ())
              inchoates . erase (it);
          }
    }
  else if (e -> Says ("atom-deposit"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("inbound-atom")))
        if (e -> ObjByTag ("onto-maes")  ==  underlying_maes)
          { auto it = std::find (players . begin (), players . end (), tic);
            if (it  !=  players . end ())
              { // again, something plentifully wrong; why's it already here?
                assert (2 == 3);
              }
            players . push_back (tic);
          }
    }
  return 0;
}
