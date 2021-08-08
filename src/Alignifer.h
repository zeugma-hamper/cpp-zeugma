
//
// (c) treadle & loam, provisioners llc
//

#ifndef ALIGNIFER_KEEPS_ITS_AXES_PRIMLY_ORIENTED
#define ALIGNIFER_KEEPS_ITS_AXES_PRIMLY_ORIENTED


#include "ZoftThing.h"

#include "TrGrappler.h"
#include "ScGrappler.h"
#include "CoGrappler.h"

#include "Node.hpp"

#include "PlatonicMaes.h"


namespace zeugma  {


class Alignifer  :  public Zeubject, public Node
{ public:
  ZoftVect loc;
  ZoftVect sca;
  Vect ovr, upp, nrm;

  void WellAndTrulyConstruct ();

  Alignifer ();
  explicit Alignifer (Renderable *ren);

  CoGrappler *AlignmentGrappler ()
    { return dynamic_cast <CoGrappler *> (FindGrappler ("alignment")); }
  ScGrappler *ScaleGrappler ()
    { return dynamic_cast <ScGrappler *> (FindGrappler ("scale")); }
  TrGrappler *LocGrappler ()
    { return dynamic_cast <TrGrappler *> (FindGrappler ("loc")); }

  ZoftVect *ScaleGrapplerScaZoftVect ()
    { if (ScGrappler *scg = ScaleGrappler ())
        return &(scg->sca);
      return NULL;
    }
  ZoftVect *ScaleGrapplerCntZoftVect ()
    { if (ScGrappler *scg = ScaleGrappler ())
        return &(scg->cnt);
      return NULL;
    }

  ZoftVect *LocGrapplerZoftVect ()
    { if (TrGrappler *trg = LocGrappler ())
        return &(trg->trans);
      return NULL;
    }

  bool InstallScaleGrapplerZoft (ZoftVect &sgz);
  bool InstallLocGrapplerZoft (ZoftVect &lgz);

  Vect CurScale ()  const
    { return sca.val; }
  Vect CurLoc ()  const
    { return loc.val; }

  ZoftVect &ScaleZoft ()
    { return sca; }
  ZoftVect &LocZoft ()
    { return loc; }

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
