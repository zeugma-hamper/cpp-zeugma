#ifndef DRAINING_THE_DESERT
#define DRAINING_THE_DESERT

#include <Zeubject.h>

#include <class_utils.hpp>

namespace charm
{

class MultiSprinkler;

// Basically, an ZeEvent publisher
class ZePublicWaterWorks : public Zeubject
{
 public:

  ZePublicWaterWorks ();

  CHARM_DELETE_MOVE_COPY(ZePublicWaterWorks);

  virtual ~ZePublicWaterWorks () { }

  virtual void Drain (MultiSprinkler *) {}
};

}

#endif //DRAINING_THE_DESERT
