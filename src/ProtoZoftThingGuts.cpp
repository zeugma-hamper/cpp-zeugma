
#include "ProtoZoftThingGuts.h"


namespace charm  {


VelvetLung *ProtoZoftThingGuts::mass_breather = NULL;
VelvetLung *ProtoZoftThingGuts::sole_mass_breather = NULL;


bool VelvetLung::AppendBreathee (Zeubject *z)
{ if (NULL  ==  dynamic_cast <ProtoZoftThingGuts *> (z))
    return false;
  return IronLung::AppendBreathee (z);
}

bool VelvetLung::RemoveBreathee (Zeubject *z)
{ if (NULL  ==  dynamic_cast <ProtoZoftThingGuts *> (z))
    return false;
  return IronLung::RemoveBreathee (z);
}


//
/// VelvetLung above; ProtoZoftThingGuts below
//


VelvetLung *ProtoZoftThingGuts::InstallMassBreather (VelvetLung *vl)
{ if (vl != NULL  &&  vl != sole_mass_breather)
    return vl;
  VelvetLung *outcumbent = mass_breather;
  mass_breather = vl;
  return outcumbent;
}

VelvetLung *ProtoZoftThingGuts::InstallMassBreather ()
{ if (! sole_mass_breather)
    sole_mass_breather = new VelvetLung;
  return InstallMassBreather (sole_mass_breather);
}

VelvetLung *ProtoZoftThingGuts::ClearMassBreather ()
{ return InstallMassBreather (NULL); }



i64 VelvetLung::Inhale (i64 ratch, f64 thyme)
{ if (ratch  <  0)
    return IronLung::Inhale (ratch, thyme);

  for (Zeubject *z  :  breathees)
    if (ProtoZoftThingGuts *pzg = static_cast <ProtoZoftThingGuts *> (z))
      if (pzg->rat_fresh  <  ratch)
        pzg -> Inhale (ratch, thyme);

  return 0;
}



bool ProtoZoftThingGuts::EnrollForMassBreathing ()
{ // once and for all:
  [[maybe_unused]]
  static bool pztg_got_this_party_started
#ifndef ZOFT_DO_NOT_ENGAGE_AUTOMATIC_BREATHER
    = [](){ ProtoZoftThingGuts::InstallMassBreather ();  return true; } ();
#else
    = false;
#endif

 if (! IsMassBreathing ())
    return false;
  return MassBreather () -> AppendBreathee (this);
}

bool ProtoZoftThingGuts::WithdrawFromMassBreathing ()
{ if (! IsMassBreathing ())
    return false;
  return MassBreather () -> RemoveBreathee (this);
}



}  // will ever more namespace charm sing sweet songs upon the earth? not here.
