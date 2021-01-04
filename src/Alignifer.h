
#ifndef ALIGNIFER_KEEPS_ITS_AXES_PRIMLY_ORIENTED
#define ALIGNIFER_KEEPS_ITS_AXES_PRIMLY_ORIENTED


#include "ZoftThing.h"

#include "TrGrappler.h"
#include "ScGrappler.h"
#include "CoGrappler.h"

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

  TrGrappler *LocGrappler ()
    { return dynamic_cast <TrGrappler *> (FindGrappler ("loc")); }
  ScGrappler *ScaleGrappler ()
    { return dynamic_cast <ScGrappler *> (FindGrappler ("scale")); }
  CoGrappler *AlignmentGrappler ()
    { return dynamic_cast <CoGrappler *> (FindGrappler ("alignment")); }

  ZoftVect *LocGrapplerZoftVect ()
    { if (TrGrappler *trg = LocGrappler ())
        return &(trg->trans);
      return NULL;
    }

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

  virtual void AlignOverUp (const Vect &ov, const Vect &up);
  virtual void AlignToOther (const Alignifer *alig)
    { if (alig)  AlignOverUp (alig -> CurOver (), alig -> CurUp ()); }
  virtual void AlignToMaes (const PlatonicMaes *maes)
    { if (maes)  AlignOverUp (maes -> Over (), maes -> Up ()); }
};


}


#endif
