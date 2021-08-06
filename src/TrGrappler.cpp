
#include "TrGrappler.h"


namespace zeugma  {


const Matrix44 &TrGrappler::PntMat ()  const
{ return pm; }


const Matrix44 &TrGrappler::NrmMat ()  const
{ return ipm; }


i64 TrGrappler::Inhale (i64 steppe, f64 thyme)
{ //trans . Inhale (steppe, thyme);
  const Vect &tr = trans . Val ();
  pm . LoadTranslation (tr.x, tr.y, tr.z);
  ipm . LoadTranslation (-tr.x, -tr.y, -tr.z);
  return 0;
}


}  // we who are about to die salute you! but does namespace zeugma?

