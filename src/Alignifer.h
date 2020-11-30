
#ifndef ALIGNIFER_KEEPS_ITS_AXES_PRIMLY_ORIENTED
#define ALIGNIFER_KEEPS_ITS_AXES_PRIMLY_ORIENTED


#include "ZoftThing.h"

#include "Node.hpp"

#include "PlatonicMaes.h"


namespace charm  {


class Alignifer  :  public Zeubject, public Node
{ public:
  ZoftVect loc;
  ZoftVect sca;

  void WellAndTrulyConstruct ();

  Alignifer ();
  explicit Alignifer (Renderable *ren);

  Vect Loc ()  const
    { return loc.val; }
  Vect Scale ()  const
    { return sca.val; }

  ZoftVect &LocZoft ()
    { return loc; }
  ZoftVect &ScaleZoft ()
    { return sca; }

  void AlignOverUp (const Vect &ov, const Vect &up);
  void AlignToMaes (PlatonicMaes *maes)
    { if (maes)  AlignOverUp (maes -> Over (), maes -> Up ()); }
};


}


#endif
