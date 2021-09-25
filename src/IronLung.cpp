
//
// (c) treadle & loam, provisioners llc
//

#include "IronLung.h"


namespace zeugma  {


bool IronLung::AppendBreathee (Zeubject *z, bool record_self_in_breathee)
{ if (! z)
    return false;
  auto erminus = breathees . end ();
  if (std::find (breathees . begin (), erminus, z)  !=  erminus)
    return false;

  aspirants . push_back (z);
  if (record_self_in_breathee)
    z -> SetBreather (this);
  return true;
}


bool IronLung::RemoveBreathee (Zeubject *z)
{ if (! z)
    return false;
  // first: if it's in aspirants, take it out.

  bool urn = false;
  auto omega = aspirants . end ();
  auto spit = std::find (aspirants . begin (), omega, z);
  if (spit  !=  omega)
    { aspirants . erase (spit);
      urn = true;
      // actually it's possible (?) that the removee is
      // both in aspirants but also already in breathees...
    }

  // and but so then now we'll look in the breathees proper:
  auto erminus = breathees . end ();
  auto it = std::find (breathees . begin (), erminus, z);
  if (it ==  erminus)
    return urn;
  else
    urn = true;

  expirees . push_back (z);  // actually schedule for removal
fprintf(stderr,"and then DITCHING BREATHEE; thus at %ld\n",breathees.size());
  if (z -> UnsecuredSac ())
    z -> SetBreather (NULL);
  return urn;
}


i64 IronLung::Inhale (i64 ratch, f64 thyme)
{ IRON_LUNG_TIDY_AROUND_ACTUAL_INHALATION ();

  for (Zeubject *z  :  breathees)
    if (z)
      z -> Inhale (ratch, thyme);
  return 0;  // uh... ahem: wouldn't this want to go last?

  IRON_LUNG_TIDY_AROUND_ACTUAL_INHALATION ();
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


}  // the discount reaper comes for us all, little namespace zeugma...
