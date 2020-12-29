
#include "Jigglegon.h"

#include "RandZoft.h"
#include "ScaleZoft.h"
#include "SumZoft.h"


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
