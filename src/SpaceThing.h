
#ifndef SPATIAL_THING_IS_BUSTING_OUT_OF_ITS_DIMENSIONAL_SLACKS
#define SPATIAL_THING_IS_BUSTING_OUT_OF_ITS_DIMENSIONAL_SLACKS


#include "base_types.hpp"

#include "GrapplerPile.h"


namespace charm  {


class SpaceThing  :  public Zeubject
{ public:
  GrapplerPile *gr_pile;

  SpaceThing ()  :  Zeubject (), gr_pile (NULL)
    { }

  ~SpaceThing ()  override
    { if (gr_pile)
        delete gr_pile;
    }

  GrapplerPile *UnsecuredGrapplerPile ()  const
    { return gr_pile; }
  GrapplerPile *AssuredGrapplerPile ()
    { return gr_pile  ?  gr_pile  :  (gr_pile = new GrapplerPile); }

  SpaceThing &AppendGrappler (Grappler *g)
    { if (g)
        AssuredGrapplerPile () -> AppendGrappler (g);
      return *this;
    }

  i64 Inhale (i64 steppe, f64 thyme)  override;
};


}  // witty semibon mots about death regarding namespace charm


#endif
