
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
  Vect ovr, upp, nrm;

  void WellAndTrulyConstruct ();

  Alignifer ();
  explicit Alignifer (Renderable *ren);

  Vect CurLoc ()  const
    { return loc.val; }
  Vect CurScale ()  const
    { return sca.val; }

  ZoftVect &LocZoft ()
    { return loc; }
  ZoftVect &ScaleZoft ()
    { return sca; }

  const Vect &CurOver ()  const
    { return ovr; }
  const Vect &CurUp ()  const
    { return upp; }
  const Vect &CurNorm ()  const
    { return nrm; }

  void AlignOverUp (const Vect &ov, const Vect &up);
  void AlignToMaes (const PlatonicMaes *maes)
    { if (maes)  AlignOverUp (maes -> Over (), maes -> Up ()); }
};


}


#endif
