
#include "Orksur.h"

#include "SinuZoft.h"
#include "SumZoft.h"


Orksur::Orksur (const PlatonicMaes &ma)  :  PlatonicMaes (ma, false),
                                            underlying_maes (&ma),
                                            sentient_dist (200.0),
                                            contact_dist (25.0)
{ }


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


Ticato *Orksur::ClosestAtom (const Vect &p)
{ Ticato *ct = NULL;
  f64 d, cd = CHRM_MAX_F64;

  for (Ticato *tic  :  players)
    if (tic)
      if ((d = tic -> Loc () . DistFrom (p))  <  cd)
        { ct = tic;  cd = d; }
  return ct;
}


i64 Orksur::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  const std::string &prv = e -> Provenance ();
  Splort *s;
  try  { s = splorts . at (prv); }
  catch (const std::out_of_range &e)
    { AppendChild (s = new Splort);
      splorts[prv] = s;
    }

  Vect n = Norm ();
  const Vect &p = e -> Loc ();
  f64 tt = fabs (n . Dot (p - loc));
  Vect proj = p  -  tt * n;
  s->loc . Set (proj);

  Vect bloff = proj - CornerBL ();
  Vect troff = CornerTR () - proj;
  s->re -> ClearAllLines ();

  const Vect &o = ovr.val, &u = upp.val;
  f64 l = o . Dot (bloff), r = o . Dot (troff);
  if (l < 0.0  ||  r < 0.0)
    return 0;
  f64 b = u . Dot (bloff), t = u . Dot (troff);
  if (b < 0.0  ||  t < 0.0)
    return 0;

  s->re -> AppendLine (proj - l * o, proj + r * o);
  s->re -> AppendLine (proj - b * u, proj + t * u);
  float aa = (tt - contact_dist) / (sentient_dist - contact_dist);
  if (aa < 0.0)
    aa = 0.0;
  else if (aa > 1.0)
    aa = 1.0;
  s->re -> SetLinesColor (ZeColor (1.0, 0.5 * (1.0 - aa)));

  auto heff = hoverees . find (prv);
  auto geff = graspees . find (prv);
assert (! (heff != hoverees . end ()  &&  geff != graspees . end ()));
  Ticato *ca = ClosestAtom (proj);

  if (! ca  ||  tt  >  sentient_dist)
    { if (heff  !=  hoverees . end ())
        { hoverees . erase (heff);
          // and whatever else's polite on un-hover
        }
      else if (geff  !=  graspees . end ())
        { graspees . erase (geff);
          // plus other leave-taking gubbish
        }
      return 0;
    }

  if (tt  <=  contact_dist)
    { if (geff  !=  graspees . end ())
        { if (geff->second.tic  ==  ca)
            { ca -> LocZoft () . Set (proj + geff->second.gropoff);
            }
          else // i.e. a different atom is 'closer', but...
            { // ... too bad! we're modally grasping an atom already
            }
        }
      else if (heff  !=  hoverees . end ())
        { Fondlish *fon = NULL;
          for (auto &fo  :  graspees)
            if (fo.second.tic  ==  ca)
              { fon = &fo.second;  break; }
          if (! fon)  // nobody else is grasping ca
            { // bid frond frarewell to whoever's in the heff, and then...
              hoverees . erase (heff);
              graspees[prv] = { ca, (ca -> Loc () - proj) };
            }
          else  // somebody else is grasping ca
            { if (heff->second.tic  !=  ca)
                heff->second.tic = ca;  // we're just going to keep hovering
            }
        }
      else
        { Fondlish *fon = NULL;
          for (auto &fo  :  graspees)
            if (fo.second.tic  ==  ca)
              { fon = &fo.second;  break; }
          if (! fon)
            { // acquiring ca as graspee without history or hovering
              graspees[prv] = { ca, (ca -> Loc () - proj) };
            }
          else  // ca already seeing some other wand...
            { // and so... we hover? uh... sure.
              hoverees[prv] = { ca, Vect::zerov };
            }
        }
/*
      if (geff and geff == tic)
        update position;
      else if (geff and geff != tic)
        uh...;
      else if (heff and (heff == tic or heff != tif))
        // what if somebody else is grasping tic?
        remove heff and add geff;
      else add geff;
*/
    }
  else  // we're between contact and hover
    { if (geff  !=  graspees . end ())
        { graspees . erase (geff);   // plus whatever else to detach grasping
          hoverees[prv] = { ca, Vect::zerov };
        }
      else if (heff  !=  hoverees . end ())
        { if (heff->second.tic  !=  ca)
            { // adios, other-tic
              heff->second.tic = ca;
            }
          // glow or other hover-y appurtenances
        }
      else
        { hoverees[prv] = { ca, Vect::zerov };
          // plus: hello hoveree!
        }
/*
      if (heff and heff == tic)
        update position;
      else if (heff and heff != tic)
        release other and make heff be tic;
      else if (geff)  // whether geff is tic or not!
        remove geff and add heff;
      else // weren't nuthin' nowhere
        add heff;
*/
    }

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
