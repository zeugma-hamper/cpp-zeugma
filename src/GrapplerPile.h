
//
// (c) treadle & loam, provisioners llc
//

#ifndef GRAPPLER_PILE_ON_YOUR_CARPET_WITH_PROBABLE_STAINS
#define GRAPPLER_PILE_ON_YOUR_CARPET_WITH_PROBABLE_STAINS


#include "Grappler.h"

#include <vector>


namespace zeugma  {


class GrapplerPile  :  public Grappler
{ public:

  Matrix44 pnt_mat, nrm_mat;
  Matrix44 inv_pnt_mat, inv_nrm_mat;
  std::vector <Grappler *> graps;

  GrapplerPile ()  :  Grappler ()
    { }
  ~GrapplerPile ()  override;

  const Matrix44 &PntMat ()  const  override;
  const Matrix44 &NrmMat ()  const  override;
  const Matrix44 &InvPntMat ()  const  override;
  const Matrix44 &InvNrmMat ()  const  override;

  i64 GrapplerCount ()  const;
  Grappler *NthGrappler (i64 ind)  const;
  i64 IndexForGrappler (Grappler *g)  const;
  i64 IndexForGrappler (const std::string &nm)  const;
  Grappler *FindGrappler (const std::string &nm)  const;

  GrapplerPile &AppendGrappler (Grappler *g);
  GrapplerPile &PrependGrappler (Grappler *g);
  GrapplerPile &InsertGrappler (Grappler *g, i64 ind);

  GrapplerPile &RemoveGrappler (Grappler *g);
  GrapplerPile &RemoveNthGrappler (i64 ind);
  GrapplerPile &RemoveAllGrapplers ();

  i64 Inhale (i64 steppe, f64 thyme)  override;
};


}  // afraid it was the salmon mousse, namespace zeugma...


#endif
