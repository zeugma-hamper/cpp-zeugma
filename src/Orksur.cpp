
#include "Orksur.h"

#include "SinuZoft.h"
#include "SumZoft.h"

#include "ZeUUID.h"

#include "vector_utils.hpp"

#include "tamparams.h"

#include <vector>


Orksur::Orksur (const PlatonicMaes &ma)  :  PlatonicMaes (ma, false),
                                            underlying_maes (&ma),
                                            collage (new Node),
                                            soncho (new SonoChoosist (&ma)),
                                            sentient_dist (200.0),
                                            contact_dist (25.0)
{ AppendChild (collage);
  AppendChild (soncho);
  soncho -> SetNumChoizls (5);
  soncho->loc . Set (Loc ()  +  0.5 * (soncho->hei + 2.0 * soncho->brd_thc
                                       - ma . Height ()) * ma . Up ());
  soncho -> Furl ();
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


void Orksur::DistinguishHoverees ()
{ static ZeColor fullc (1.0, 1.0);
  static ZeColor fadec (0.4, 1.0);

  bool never_mind = (hoverees . size ()  ==  0);
  for (Ticato *tic  :  players)
    if (tic)
      tic->re -> SetAdjColor (never_mind ? fullc : fadec);

  for (auto &f  :  hoverees)
    if (f.second.tic)
      f.second.tic->re -> SetAdjColor (fullc);
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
    . Set (Vect (Tamparams::Current ()->table_scale_factor));

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
  f64 tt = n . Dot (p - loc);
  Vect proj = p  -  tt * n;
  s->loc . Set (proj);
  tt = fabs (tt);

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

  if (tt  >  sentient_dist)  // but wait... what if grasping?
    { if (heff  !=  hoverees . end ())
        { hoverees . erase (heff);
          // and whatever else's polite on un-hover
        }
      else if (geff  !=  graspees . end ())
        { graspees . erase (geff);
          // plus other leave-taking gubbish
        }
      DistinguishHoverees ();
      return 0;
    }

  // we'll give the sonochoosist first dibs
  if (soncho)
    if (soncho -> ZESpatialMove (e)  >  0)
      return 1;

  if (! ca)
    return 0;  // but when would... when would this actually...

  if (tt  <=  contact_dist)
    { if (geff  !=  graspees . end ())
        { if (geff->second.tic  ==  ca)
            { ca -> LocZoft () . Set (proj + geff->second.gropoff);
              // 'ca' is the same as 'geff.second.tic', as below
            }
          else // i.e. a different atom is 'closer', but...
            { // ... too bad! we're modally grasping an atom already;
              // thus still manipulate the grasped atom
              geff->second.tic
                -> LocZoft () . Set (proj + geff->second.gropoff);
            }
        }
      else if (heff  !=  hoverees . end ())
        { if (heff->second.tic  ==  ca)  // expected case; same was hovered
            { hoverees . erase (heff);
              graspees[prv] = { ca, (ca -> CurLoc () - proj) };
              ca -> MakeRenderablesForemostInLayer ();
              AtomicFirstStrike (ca);
            }
          else  // weird, but possible
            { Fondlish *fon = NULL;
              for (auto &gr  :  graspees)
                if (gr.second.tic  ==  ca)
                  { fon = &gr.second;  break; }
              if (! fon)  // nobody else is grasping ca (which != the hoveree)
                { // bid frond frarewell to whoever's in the heff, and then...
                  hoverees . erase (heff);
                  graspees[prv] = { ca, (ca -> CurLoc () - proj) };
                  ca -> MakeRenderablesForemostInLayer ();
                  AtomicFirstStrike (ca);
                }
              else  // somebody else is grasping ca
                { // if (heff->second.tic  !=  ca)  // can't be the case
                  heff->second.tic = ca;  // so we'll just keep hovering
                }
            }
        }
      else
        { Fondlish *fon = NULL;
          for (auto &fo  :  graspees)
            if (fo.second.tic  ==  ca)
              { fon = &fo.second;  break; }
          if (! fon)
            { // acquiring ca as graspee without history or hovering
              graspees[prv] = { ca, (ca -> CurLoc () - proj) };
            }
          else  // ca already seeing some other wand...
            { // and so... we hover? uh... sure.
              hoverees[prv] = { ca, Vect::zerov };
            }
        }
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
    }

  DistinguishHoverees ();
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
              { inchoates . push_back (tic);
                tic->accom_sca
                  . Set (Vect (Tamparams::Current ()->table_scale_factor));
              }
          }
        else if (e -> ObjByTag ("from-maes")  ==  underlying_maes)
          { auto it = std::find (inchoates . begin (), inchoates . end (), tic);
            if (it  != inchoates . end ())
              { inchoates . erase (it);
                tic->accom_sca . Set (Vect (1.0));
              }
          }
    }
  else if (e -> Says ("atom-deposit"))
    { if (tic = dynamic_cast <Ticato *> (e -> ObjByTag ("inbound-atom")))
        if (e -> ObjByTag ("onto-maes")  ==  underlying_maes)
          { AppendAtomToCollage (tic);
            tic -> BBoxSetColor (Tamglobals::Only ()->tabatom_bbox_color);
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
