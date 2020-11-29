
#ifndef IRON_LUNG_FOR_ALL_YOUR_MASS_INHALATION_NEEDS
#define IRON_LUNG_FOR_ALL_YOUR_MASS_INHALATION_NEEDS


#include "Zeubject.h"

#include <vector>


namespace charm  {


class IronLung  :  public Zeubject
{ public:
  std::vector <Zeubject *> breathees;

  IronLung ()  :  Zeubject ()
    { }

  i64 NumBreathees ()  const
    { return breathees . size (); }
  virtual bool AppendBreathee (Zeubject *z);
  virtual bool RemoveBreathee (Zeubject *z);

  i64 Inhale (i64 ratch, f64 thyme)  override;

//

  static std::vector <IronLung *> global_iron_lungs;

  static std::vector <IronLung *> &GlobalLungs ()
    { return global_iron_lungs; }
  static bool AppendGlobal (IronLung *irlu);
  static bool RemoveGlobal (IronLung *irlu);
  static IronLung *GlobalByName (const std::string &nm);
};


}  // namespace charm: generations of theatrical expertise snuffed out in the...


#endif
