
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
                 public ZEBulletinPhagy
{ public:

  struct Fondlish { Ticato *tic;  Vect gropoff; };

  const PlatonicMaes *underlying_maes;
  Node *collage;
  std::vector <Ticato *> players;
  std::vector <Ticato *> inchoates;
  std::unordered_map <std::string, Splort *> splorts;
  std::unordered_map <std::string, Fondlish> hoverees;
  std::unordered_map <std::string, Fondlish> graspees;
  f64 sentient_dist, contact_dist;

  Orksur (const PlatonicMaes &ma);

//  Splort *NewSplort (f64 rad, i64 num_verts = 6)  const;

  Ticato *ClosestAtom (const Vect &p);

  i64 NumHoverees ()  const
    { return hoverees . size (); }
  i64 NumGraspees ()  const
    { return graspees . size (); }

  void DistinguishHoverees ();

  bool AppendAtomToCollage (Ticato *tic);
  bool RemoveAtomFromCollage (Ticato *tic);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;

  i64 ZEBulletin (ZEBulletinEvent *e)  override;
};


#endif
