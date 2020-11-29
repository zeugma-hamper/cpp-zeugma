
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
//fprintf(stderr,"ADDING BREATHEE --- now at %ld\n",breathees.size());
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
//fprintf(stderr,"and then DITCHING BREATHEE; thus at %ld\n",breathees.size());
  return true;
}


i64 IronLung::Inhale (i64 ratch, f64 thyme)
{ for (Zeubject *z  :  breathees)
    if (z)
      z -> Inhale (ratch, thyme);
  return 0;
}


//
/// IronLung's luscious static instances and methods
//

std::vector <IronLung *> IronLung::global_iron_lungs;

bool IronLung::AppendGlobal (IronLung *irlu)
{ if (irlu  &&
      std::find (global_iron_lungs . begin (), global_iron_lungs . end (), irlu)
        ==  global_iron_lungs . end ())
    { global_iron_lungs . push_back (irlu);
      return true;
    }
  return false;
}


bool IronLung::RemoveGlobal (IronLung *irlu)
{ auto it = std::find (global_iron_lungs . begin (),
                       global_iron_lungs . end (),
                       irlu);
  if (it  !=  global_iron_lungs . end ())
    { global_iron_lungs . erase (it);
      return true;
    }
  return false;
}


IronLung *IronLung::GlobalByName (const std::string &nm)
{ for (IronLung *irlu  :  global_iron_lungs)
    if (irlu  &&  nm == irlu -> Name ())
      return irlu;
  return NULL;
}


static bool whiff_of_poo
  = [] () { IronLung *ol = new IronLung;  ol -> SetName ("omni-lung");
            IronLung::AppendGlobal (ol);  return true; } ();


}  // the discount reaper comes for us all, little namespace charm...
