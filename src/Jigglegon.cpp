
#include "Jigglegon.h"

#include "RandZoft.h"
#include "ScaleZoft.h"
#include "SumZoft.h"

#include "TrGrappler.h"


Jigglegon::Jigglegon ()  :  Alignifer (),
                            re (new PolygonRenderable), amp_sca (1.0)
{ re -> SetEdgeColor (ZeColor (1.0, 0.15));
  re -> SetShouldEdge (true);
  re -> SetShouldFill (false);

  fadist . SetInterpTime (0.93);
  fadist . SetHard (ZeColor (1.0, 1.0));
  re -> AdjColorZoft () . BecomeLike (fadist);

  voits . resize (4);
  AppendRenderable (re);
}


void Jigglegon::SetCorners (ZoftVect &az, ZoftVect &bz,
                            ZoftVect &cz, ZoftVect &dz)
{ if (NumVertices ()  !=  4)
    SetNumVertices (4);
  voits[0] . BecomeLike (az);
  voits[1] . BecomeLike (bz);
  voits[2] . BecomeLike (cz);
  voits[3] . BecomeLike (dz);
}

void Jigglegon::SetCorners (const Vect &a, const Vect &b,
                            const Vect &c, const Vect &d)
{ if (NumVertices ()  !=  4)
    SetNumVertices (4);
  voits[0] . Set (a);
  voits[1] . Set (b);
  voits[2] . Set (c);
  voits[3] . Set (d);
}

void Jigglegon::Populate (u32 num_winds, const Vect &ampl)
{ re -> ClearVertices ();
  u64 nv = NumVertices ();
  for (  ;  num_winds > 0  ;  --num_winds)
    for (u64 q = 0  ;  q < nv  ;  ++q)
      { RandVect jigl ((ZoftVect (ampl))); // fer vexing freak's sake...
        ScaleVect sca_jig (jigl, amp_sca);
        SumVect vrt (voits[q], sca_jig);
        re -> AppendVertex (vrt);
      }
}


void Jigglegon::SetLoc (const Vect &l)
{ if (TrGrappler *tgr = dynamic_cast <TrGrappler *> (FindGrappler ("loc")))
    tgr -> SetTranslation (l);
  else
    fprintf (stderr, "Jigglegon::SetLoc() -- no translation Grappler...\n");
}


void Jigglegon::HitchLocTo (const ZoftVect &lz)
{ if (TrGrappler *tgr = dynamic_cast <TrGrappler *> (FindGrappler ("loc")))
    tgr -> TranslationZoft () . BecomeLike (lz);
  else
    fprintf (stderr, "Jigglegon::HitchLocTo() -- no translation Grappler...\n");
}
