
#ifndef SPECTACLE_CAUSEWAY_IS_NOT_THE_ROAD_YOUR_OPTOMETRIST_DRIVES_ON
#define SPECTACLE_CAUSEWAY_IS_NOT_THE_ROAD_YOUR_OPTOMETRIST_DRIVES_ON


#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"
#include "ZEBulletinEvent.h"

#include "GeomFumble.h"

#include "PlatonicMaes.h"

#include "Node.hpp"

#include <vector>


namespace charm  {


struct Swath
{ G::Segment prone;
  G::Segment plumb;
  PlatonicMaes *supporting_maes;
  Swath *nxt, *prv;
  Swath ()  :  supporting_maes (NULL), nxt (NULL), prv (NULL)  { }
  Swath (const G::Segment &ro, const G::Segment &lu, PlatonicMaes *ma)
     :  prone (ro), plumb (lu), supporting_maes (ma), nxt (NULL), prv (NULL)
    { }
};


class SpectacleCauseway  :  public Zeubject,
                            public ZESpatialPhagy,
                            public ZEYowlPhagy,
                            public ZEBulletinPhagy
{ public:
  std::vector <PlatonicMaes *> underlying_maeses;
  std::vector <Swath *> meander;
  Node *amok_field;

  SpectacleCauseway ();

  void AppendSwath (Swath *sw);
  Swath *SwathFor (const PlatonicMaes *ma);

  Node *AmokField ()
    { return amok_field; }
  void SetAmokField (Node *af)
    { amok_field = af; }

  f64 MeanderLength ();
  std::pair <PlatonicMaes *, Vect>
    MaesAndPosFromMeanderDist (f64 d, bool clamp_to_ends = true);
};


}


#endif
