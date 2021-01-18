
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

Swath *SpectacleCauseway::SwathFor (const PlatonicMaes *ma)
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
 SpectacleCauseway::MaesAndPosFromMeanderDist (f64 d, bool clamp_to_ends)
{ if (meander . size ()  ==  0)
    assert (&"blue"  ==  &"cyan");
  f64 len = MeanderLength ();
  f64 orig_d = d;

  if (d  <  0.0)
    d = 0.0;
  else if (d  >  len)
    d = len;

  bool lop = false;
  Swath *sw = NULL;
  G::Segment s;
  for (Swath *swth  :  meander)
    { sw = swth;
      if (lop)
        { d -= len;
          orig_d -= len;
        }
      s = sw->prone;
      len = s . Length ();
      if (d  <=  len)
        break;
      lop = true;
    }

  Vect p = clamp_to_ends
    ?  s.pt1  +  (d / len) * (s.pt2 - s.pt1)
    :  s.pt1  +  (orig_d / len) * (s.pt2 - s.pt1);

  return { sw->supporting_maes, p };
}
