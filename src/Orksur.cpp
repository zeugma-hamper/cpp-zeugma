
#include "Orksur.h"

#include "SinuZoft.h"
#include "SumZoft.h"


Orksur::Orksur (const PlatonicMaes &ma)  :  PlatonicMaes (ma, false)
{ }


Splort *Orksur::NewSplort (f64 rad, i64 num_verts)  const
{ Splort *spl = new Splort;

  for (i64 q = 0  ;  q < num_verts  ;  ++q)
    { f64 theeeta = 2.0 * M_PI / (f64)num_verts * (f64)q;
      Vect radial = cos (theeeta) * ovr  +  sin (theeeta) * upp;
      SinuVect arm (0.13 * rad * radial, 1.0 + 0.3 * drand48 (),
                    0.333 * (1.0 + 2.0 * (q%2)) * rad * radial);
      SumVect voitecks (spl->loc, arm);
      spl->ren -> AppendVertex (voitecks);
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
    { AppendChild (s = NewSplort (100.0));
      splorts[prv] = s;
    }

  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 t = n . Dot (p - loc);
  Vect proj = p  -  t * n;
  s->loc . Set (proj);

  return 0;
}
