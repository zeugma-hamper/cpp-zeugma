
#ifndef ORKSUR_SURELY_CANNOT_HAVE_ANYTHING_TO_DO_WITH_A_WORK_SURFACE
#define ORKSUR_SURELY_CANNOT_HAVE_ANYTHING_TO_DO_WITH_A_WORK_SURFACE


#include "PlatonicMaes.h"

#include "Node.hpp"
#include "PolygonRenderable.h"
#include "LinePileRenderable.h"

#include "ZESpatialEvent.h"
#include "ZEBulletinEvent.h"
#include "ZEYowlEvent.h"

#include "Ticato.h"
#include "SonoChoosist.h"
#include "AudioMessenger.hpp"
#include "Jigglegon.h"

#include <map>
#include <unordered_map>


using namespace charm;


struct Splort  :  Node
{ ZoftVect loc;
  LinePileRenderable *re;
  Splort ()  :  Node (),
                re (new LinePileRenderable)
    { loc . MakeBecomeLikable ();
      re -> SetLinesColor (ZeColor (1.0, 0.05));
      AppendRenderable (re);
    }
};


class Orksur  :  public PlatonicMaes, public Node,
                 public ZESpatialPhagy, public ZEYowlPhagy,
                 public ZEBulletinPhagy,
                 public TASSuggestionEvent::TASSuggestionPhage
{ public:

  struct Fondlish { Ticato *tic;  Vect gropoff; };

  const PlatonicMaes *underlying_maes;
  Node *assembly;
  SonoChoosist *soncho;
  PlatonicMaes *associated_wallmaes;
  i64 ascension_phase;
  Alignifer *ascending_collage;
  std::vector <Ticato *> players;
  std::vector <Ticato *> inchoates;
  std::unordered_map <std::string, Splort *> splorts;
  std::unordered_map <std::string, Fondlish> hoverees;
  std::unordered_map <std::string, Fondlish> graspees;
  f64 sentient_dist, contact_dist;
  std::unordered_map <std::string, f64> prev_prox_by_prov;
  std::map <u64, Ticato *> awaiting_audio_sooth;
  Ticato *sel_atom;

  static std::vector <Jigglegon *> fallow_jigs;
//  static std::vector <Jigglegon *> active_jigs;

  Orksur (const PlatonicMaes &ma);

//  Splort *NewSplort (f64 rad, i64 num_verts = 6)  const;

  stringy_list CollageAtomsNameList ();

  Alignifer *PermaFixCollage ();
  void EffectAscension ();

  bool CurrentlyAscending ()
    { return (ascension_phase  >=  0); }
  bool AscensionPhaseJustNowDone ();
  void EffectNextAscensionPhase ();

  Jigglegon *FurnishFreeJiggler ();
  void ReturnJigglerToShelf (Jigglegon *jig);

  void AtomicFirstStrike (Ticato *tic);  // called when an atom's first touched
  void AtomicFinalGutter (Ticato *tic);  // for the calling on soften

  Ticato *ClosestAtom (const Vect &p);

  i64 NumHoverees ()  const
    { return hoverees . size (); }
  i64 NumGraspees ()  const
    { return graspees . size (); }

  void DistinguishManipulees ();

  bool AppendAtomToCollage (Ticato *tic);
  bool RemoveAtomFromCollage (Ticato *tic);

  bool AtomIsGrasped (Ticato *tic)
    { if (tic)
        for (auto gr  :  graspees)  if (gr.second.tic  ==  tic)  return true;
      return false;
    }

  void SilenceAllAtoms ();

  AtomThusness ImpelFreeAtom (Ticato *tic, f64 dt);

  void DisposeOfCollage ();
  void EliminateCollage ();

  void WhisperSplortily (const std::string &prv, const Vect &proj, f64 dst);

  i64 ConfectSpatialPointingFromPosition (ZESpatialMoveEvent *e);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;

  i64 ZEYowlAppear (ZEYowlAppearEvent *e)  override;
  i64 ZEBulletin (ZEBulletinEvent *e)  override;
  i64 TASSuggestion (TASSuggestionEvent *e)  override;

  i64 Inhale (i64 ratch, f64 thyme)  override;
};


#endif
