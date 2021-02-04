
#ifndef VELVET_LUNG_IS_LIKE_IRON_BUT_YOU_KNOW_RATHER_MORE_ELLIPSIS_ZOFT
#define VELVET_LUNG_IS_LIKE_IRON_BUT_YOU_KNOW_RATHER_MORE_ELLIPSIS_ZOFT


#include "IronLung.h"


namespace charm  {


class ProtoZoftThingGuts;


class VelvetLung  :  public IronLung
{ public:

  bool AppendBreathee (Zeubject *z,
                       bool record_self_in_breathee = false)  override;
  bool RemoveBreathee (Zeubject *z)  override;

  i64 Inhale (i64 ratch, f64 thyme)  override;
};



class ProtoZoftThingGuts  :  public Zeubject
{ public:
  i64 rat_fresh;
  i32 num_hosts;

  static VelvetLung *mass_breather;
  static VelvetLung *sole_mass_breather;

  static bool IsMassBreathing ()
    { return (mass_breather != NULL); }
  static VelvetLung *MassBreather ()
    { return mass_breather; }
  static VelvetLung *InstallMassBreather (VelvetLung *vl);
  static VelvetLung *InstallMassBreather ();
  static VelvetLung *ClearMassBreather ();

  ProtoZoftThingGuts ()  :  Zeubject (), rat_fresh (-1), num_hosts (0)
    { }

  // inside the following method is this:
  // "static bool pztg_got_this_party_started;"
  // the cpp-side definition of which foregoing controls whether
  // each Zoft from the git-go is automatically inhaled...

  bool EnrollForMassBreathing ();
  bool WithdrawFromMassBreathing ();
};


}  //  like roy batty, namespace charm wants more life. but: no.


#endif
