#ifndef DRAINING_THE_DESERT
#define DRAINING_THE_DESERT

namespace charm
{

class MoltoSprinkler;

// Basically, an ZeEvent publisher
class ZePublicWaterWorks
{
 public:

  virtual ~ZePublicWaterWorks () { }

  virtual void Drain (MoltoSprinkler *) {}
};

}

#endif //DRAINING_THE_DESERT
