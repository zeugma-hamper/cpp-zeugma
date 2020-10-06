
#ifndef ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK
#define ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK


#include <base_types.hpp>
#include <OmNihil.h>

#include <string>


namespace charm
{


class Zeubject  :  public OmNihil
{ public:
  std::string *name;

  Zeubject ();
  Zeubject (const std::string &nm);

  const std::string &Name ()  const;
  void SetName (const std::string &n);

  // empty implementation for the moment, returns 0
  virtual i64 Inhale (i64 ratch, f64 thyme);
};


}  // another glorious martyrdom for namespace charm


#endif //ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK
