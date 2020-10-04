
#ifndef RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS
#define RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS



#include "Zeubject.h"

#include "ZeEvent.h"

#include <lo/lo_cpp.h>


namespace charm  {


class RawEventParser  :  public Zeubject
{ public:
  EventSprinklerGroup *esg;
  RawEventParser ()  :  Zeubject (), esg (NULL)
    { }
  EventSprinklerGroup *SprinklerGroup ()
    { return esg; }
  RawEventParser &InstallSprinklerGroup (EventSprinklerGroup *sprigrou)
    { esg = sprigrou;  return *this; }
};


class RawOSCWandParser  :  public RawEventParser
{ public:
  void Parse (const std::string &path, const lo::Message &m,
              OmNihil *phage = NULL);
};


}  // when your little namespace charm is on the slab, where will it tickle you?


#endif
