
#include "CoGrappler.h"


using namespace charm;


void CoGrappler::SetViaNormalizedBasisVectors (const Vect &e0, const Vect &e1,
                                               const Vect &e2)
{ pm . Load (e0.x, e0.y, e0.z, 0.0,
             e1.x, e1.y, e1.z, 0.0,
             e2.x, e2.y, e2.z, 0.0,
             0.0, 0.0, 0.0, 1.0);
  ipm . Load (e0.x, e1.x, e2.x, 0.0,
              e0.y, e1.y, e2.y, 0.0,
              e0.z, e1.z, e2.z, 0.0,
              0.0, 0.0, 0.0, 1.0);
  nm . Load (pm);
  inm . Load (ipm);
}
