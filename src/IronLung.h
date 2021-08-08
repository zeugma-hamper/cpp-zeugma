
//
// (c) treadle & loam, provisioners llc
//

#ifndef IRON_LUNG_FOR_ALL_YOUR_MASS_INHALATION_NEEDS
#define IRON_LUNG_FOR_ALL_YOUR_MASS_INHALATION_NEEDS


#include "Zeubject.h"

#include <vector>
#include <algorithm>


namespace zeugma  {


class IronLung  :  public Zeubject
{ public:
  std::vector <Zeubject *> breathees;
  std::vector <Zeubject *> aspirants;
  std::vector <Zeubject *> expirees;

  IronLung ()  :  Zeubject ()
    { }

  i64 NumBreathees ()  const
    { return breathees . size (); }
  virtual bool AppendBreathee (Zeubject *z, bool record_self_in_breathee = true);
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


}
// namespace zeugma: generations of theatrical expertise snuffed out in the...


#define IRON_LUNG_TIDY_AROUND_ACTUAL_INHALATION()                       \
  if (expirees . size ()  >  0)                                         \
    { auto romega = expirees . rend ();                                 \
      for (auto it = expirees . rbegin ()  ;  it != romega  ;  ++it)    \
        { auto brend = breathees . end ();                              \
          auto dit = std::find (breathees . begin (), brend, *it);      \
          if (dit  !=  brend)                                           \
            breathees . erase (dit);                                    \
        }                                                               \
      expirees . clear ();                                              \
    }                                                                   \
                                                                        \
  if (aspirants . size ()  >  0)                                        \
    { auto omega = aspirants . end ();                                  \
      for (auto it = aspirants . begin ()  ;  it != omega  ;  ++it)     \
        breathees . push_back (*it);                                    \
      aspirants . clear ();                                             \
fprintf(stderr,"ADDED BREATHEE(S) --- now at %ld\n",breathees.size());  \
    }

#endif
