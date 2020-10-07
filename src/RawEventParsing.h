
#ifndef RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS
#define RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS



#include "Zeubject.h"
#include "ZeEvent.h"

#include "MultiSprinkler.hpp"

#include <lo/lo_cpp.h>


namespace charm  {


class RawEventParser  :  public Zeubject
{ public:
  EventSprinklerGroup *esg;
  MultiSprinkler *mulspri;
  RawEventParser ()  :  Zeubject (), esg (NULL)
    { }
  EventSprinklerGroup *SprinklerGroup ()
    { return esg; }
  void InstallSprinklerGroup (EventSprinklerGroup *sprigrou)
    { esg = sprigrou; }
  MultiSprinkler *SoleMultiSprinkler ()
    { return mulspri; }
  void InstallMultiSprinkler (MultiSprinkler *ms)
    { mulspri = ms; }
};


class RawOSCWandParser  :  public RawEventParser
{ public:
  void Parse (const std::string &path, const lo::Message &m,
              OmNihil *phage = NULL);
};


}  // when your little namespace charm is on the slab, where will it tickle you?


#endif
