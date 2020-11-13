
#ifndef ORKSUR_SURELY_CANNOT_HAVE_ANYTHING_TO_DO_WITH_A_WORK_SURFACE
#define ORKSUR_SURELY_CANNOT_HAVE_ANYTHING_TO_DO_WITH_A_WORK_SURFACE


#include "PlatonicMaes.h"

#include "Node.hpp"
#include "PolygonRenderable.h"

#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"

#include <unordered_map>


using namespace charm;


struct Splort  :  Node
{ ZoftVect loc;
  PolygonRenderable *re1, *re2;
  Splort ()  :  Node (),
                re1 (new PolygonRenderable), re2 (new PolygonRenderable)
    { loc . MakeBecomeLikable ();
      AppendRenderable (re1);
      AppendRenderable (re2);
    }
};


class Orksur  :  public PlatonicMaes, public Node,
                 public ZESpatialPhagy, public ZEYowlPhagy
{ public:
  std::unordered_map <std::string, Splort *> splorts;

  Orksur (const PlatonicMaes &ma);

  Splort *NewSplort (f64 rad, i64 num_verts = 6)  const;

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
};


#endif
