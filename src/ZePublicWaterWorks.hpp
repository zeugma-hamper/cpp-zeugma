
#ifndef DRAINING_THE_DESERT
#define DRAINING_THE_DESERT


#include <Zeubject.h>

#include <class_utils.hpp>


namespace charm
{


class MultiSprinkler;


// Basically, a ZeEvent publisher
// is this true? ------^
// is not the M'Sprinkler in fact the publisher?
// a WaterWorks is rather the collection network (of which there
// are possibly several) that feeds into the publisher, no?
// ...
// looking now at the implementation (in GLFW-ww) of Drain(),
// musing that that method's name is a miscue, suggesting that
// events are accumulated and later distributed; but in fact
// it seems that no event is ever collected and buffered; all
// are sent right along the instant they're "received".
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
