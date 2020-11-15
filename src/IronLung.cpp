
#include "IronLung.h"

#include <algorithm>


namespace charm  {


bool IronLung::AppendBreathee (Zeubject *z)
{ if (! z)
    return false;
  auto erminus = breathees . end ();
  if (std::find (breathees . begin (), erminus, z)  !=  breathees . end ())
    return false;
  breathees . push_back (z);
fprintf(stderr,"ADDING BREATHEE --- now at %ld\n",breathees.size());
  return true;
}


bool IronLung::RemoveBreathee (Zeubject *z)
{ if (! z)
    return false;
  auto erminus = breathees . end ();
  auto it = std::find (breathees . begin (), erminus, z);
  if (it ==  erminus)
    return false;
  breathees . erase (it);
fprintf(stderr,"and then DITCHING BREATHEE; thus at %ld\n",breathees.size());
  return true;
}


i64 IronLung::Inhale (i64 ratch, f64 thyme)
{ for (Zeubject *z  :  breathees)
    if (z)
      z -> Inhale (ratch, thyme);
  return 0;
}


}  // the discount reaper comes for us all, little namespace charm...
