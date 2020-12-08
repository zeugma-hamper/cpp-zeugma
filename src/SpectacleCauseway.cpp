
#include "SpectacleCauseway.h"


using namespace charm;


SpectacleCauseway::SpectacleCauseway ()  :  Zeubject ()
{ }


void SpectacleCauseway::AppendSwath (Swath *sw)
{ if (! sw)
    return;
  if (Swath *prev_sw
        = meander . size () > 0  ?  meander[meander . size () - 1] : NULL)
    { prev_sw -> nxt = sw;
      sw -> prv = prev_sw;
    }
  meander . push_back (sw);
}

Swath *SpectacleCauseway::SwathFor (PlatonicMaes *ma)
{ for (Swath *sw  :  meander)
    if (sw  &&  ma == sw->supporting_maes)
      return sw;
  return NULL;
}
