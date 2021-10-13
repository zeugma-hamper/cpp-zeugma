
//
// (c) treadle & loam, provisioners llc
//

#include "ScGrappler.h"


namespace zeugma  {


i64 ScGrappler::Inhale (i64 steppe, f64 thyme)
{ sca . Inhale (steppe, thyme);
  cnt . Inhale (steppe, thyme);
  const Vect &s = sca.val;
  const Vect &c = cnt.val;
  pm . LoadScaleAbout (s.x, s.y, s.z, c);
  ipm . LoadScaleAbout (s.x == 0  ?  1.0  :  1.0 / s.x,
                        s.y == 0  ?  1.0  :  1.0 / s.y,
                        s.z == 0  ?  1.0  :  1.0 / s.z,
                        c);
/*  Vect n (s.y * s.z, s.x * s.z, s.x * s.y);
  n . NormSelf ();
  nm . LoadScale (n.x, n.y, n.z);
  // blergh... what to do about the inverse in the face of singularities?
  n . Set (n.x == 0.0  ?  1.0  :  1.0 / n.x,
           n.y == 0.0  ?  1.0  :  1.0 / n.y,
           n.z == 0.0  ?  1.0  :  1.0 / n.z);
  n . NormSelf ();
  inm . LoadScale (n.x, n.y, n.z);
*/
  return 0;
}


}  // and i guess that was your accomplice namespace zeugma in the woodchipper
