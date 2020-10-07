#ifndef DRAINING_THE_DESERT
#define DRAINING_THE_DESERT

namespace charm
{

class MultiSprinkler;

// Basically, an ZeEvent publisher
class ZePublicWaterWorks
{
 public:

  virtual ~ZePublicWaterWorks () { }

  virtual void Drain (MultiSprinkler *) {}
};

}

#endif //DRAINING_THE_DESERT
