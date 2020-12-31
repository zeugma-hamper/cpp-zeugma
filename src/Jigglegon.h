
#ifndef JIGGLEGON_IS_HARDLY_LIKE_UNTO_JELLO_BRAND_GELATIN
#define JIGGLEGON_IS_HARDLY_LIKE_UNTO_JELLO_BRAND_GELATIN


#include "Alignifer.h"

#include "PolygonRenderable.h"


using namespace charm;


struct Jigglegon  :  Alignifer
{ PolygonRenderable *re;
  std::vector <ZoftVect> voits;
  ZoftFloat amp_sca;

  Jigglegon ();

  i64 NumVertices ()  const
    { return voits . size (); }
  void SetNumVertices (u32 nv)
    { if (nv > 1)  voits . resize (nv); }
  ZoftVect &NthVertex (i64 ind)
    { if (ind >= 0  &&  ind < NumVertices ())  return voits[ind];
      throw std::range_error ("Jigglegon: how about a decent index?");
    }

  void SetCorners (ZoftVect &az, ZoftVect &bz, ZoftVect &cz, ZoftVect &dz);
  void SetCorners (const Vect &a, const Vect &b, const Vect &c, const Vect &d);
  void Populate (u32 num_winds, const Vect &ampl);

  void SetLoc (const Vect &l);
  void HitchLocTo (const ZoftVect &lz);
};


#endif
