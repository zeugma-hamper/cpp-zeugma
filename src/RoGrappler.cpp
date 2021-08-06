
#include "RoGrappler.h"


namespace zeugma  {


i64 RoGrappler::Inhale (i64 steppe, f64 thyme)
{ cnt . Inhale (steppe, thyme);
  axs . Inhale (steppe, thyme);
  ang . Inhale (steppe, thyme);
  pha . Inhale (steppe, thyme);

  const Vect &ce = cnt.val;
  Vect ax = axs.val . Norm ();
  f64 an = ang.val;
  f64 ph = pha.val;

  nm . LoadRotationPreNormed (ax, an + ph);
  inm . LoadRotationPreNormed (-ax, an + ph);

  Matrix44 shimmy;
  shimmy.m[3][0] = -ce.x;
  shimmy.m[3][1] = -ce.y;
  shimmy.m[3][2] = -ce.z;

  pm . Load (nm) . PreMulSelfBy (shimmy);
  ipm . Load (inm) . PreMulSelfBy (shimmy);

  shimmy.m[3][0] = ce.x;
  shimmy.m[3][1] = ce.y;
  shimmy.m[3][2] = ce.z;

  pm . MulSelfBy (shimmy);
  ipm . MulSelfBy (shimmy);

  return 0;
}


}
//  roy "namespace zeugma" batty says all those memories will be lost, like...
