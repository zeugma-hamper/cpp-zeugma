
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

#include "tamparams.h"

#include "bgfx_utils.hpp"

#include <map>
#include <unordered_map>


using namespace charm;


class GraumanPalace;


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


#define ASCPH_TABLE_SLIDE  0
#define ASCPH_FIRST_RISE   1
#define ASCPH_ENBLOATEN    2
#define ASCPH_BEFORE_PRESO 3
#define ASCPH_PRESENTATION 4
#define ASCPH_AFTER_PRESO  5
#define ASCPH_ENSVELTEN    6
#define ASCPH_SECOND_RISE  7
#define ASCPH_ENTER_HEAVEN 8


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
  GraumanPalace *associated_cinelib;
  i64 ascension_phase;
  Alignifer *ascending_collage;
  OeuvreAfterlife *valhalla;
  std::vector <Ticato *> players;
  std::vector <Ticato *> inchoates;
  std::unordered_map <std::string, Splort *> splorts;
  std::unordered_map <std::string, Fondlish> hoverees;
  std::unordered_map <std::string, Fondlish> graspees;
  f64 sentient_dist, contact_dist;
  std::unordered_map <std::string, f64> prev_prox_by_prov;
  std::map <u64, Ticato *> awaiting_audio_sooth;
  Ticato *sel_atom;

  InterpVect asc_table_slide;
  InterpVect asc_first_rise;
  InterpVect asc_perf_bloat;
  MotherTime asc_hold_zeit;
  MotherTime asc_perf_zeit;
  InterpVect asc_final_rise;

  static std::vector <Jigglegon *> fallow_jigs;

  Orksur (const PlatonicMaes &ma);

  OeuvreAfterlife *RetrieveValhalla ()
    { if (! valhalla)
        valhalla = Tamglobals::Only ()->valhalla;
      return valhalla;
    }

  GraumanPalace *AssociatedCinelib ()
    { return associated_cinelib; }
  void SetAssociatedCinelib (GraumanPalace *gp)
    { associated_cinelib = gp; }

  stringy_list CollageAtomsNameList ();

  Alignifer *PermaFixCollage ();
  void EffectAscension ();

  bool CurrentlyAscending ()
    { return (ascension_phase  >=  0); }
  void ConcludeAscension ();
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
