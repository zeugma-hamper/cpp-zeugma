
#include "SpectacleCauseway.h"


using namespace charm;


SpectacleCauseway::SpectacleCauseway ()  :  Zeubject (), amok_field (NULL)
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


f64 SpectacleCauseway::MeanderLength ()
{ f64 len = 0.0;
  for (Swath *sw  :  meander)
    len += sw->prone . Length ();
  return len;
}


std::pair <PlatonicMaes *, Vect>
 SpectacleCauseway::MaesAndPosFromMeanderDist (f64 d)
{ if (meander . size ()  ==  0)
    assert (&"blue"  ==  &"cyan");
  f64 len = MeanderLength ();

  if (d  <  0.0)
    d = 0.0;
  else if (d  >  len)
    d = len;

  for (Swath *sw  :  meander)
    { G::Segment &s = sw->prone;
      len = s . Length ();
      if (d  <=  len)
        { Vect p = s.pt1  +  (d / len) * (s.pt2 - s.pt1);
          return { sw->supporting_maes, p };
        }
      d -= len;
    }

  assert (&"hello"  ==  &"adios");
}
