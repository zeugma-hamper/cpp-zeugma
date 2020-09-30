
#ifndef ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK
#define ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK


#include <base_types.hpp>
#include <OmNihil.h>

#include <string>
#include <string_view>

namespace charm
{

class Zeubject  :  public OmNihil
{ public:
  std::string name;

  Zeubject ();
  Zeubject (std::string_view name);

  const std::string &Name ()  const;
  void SetName (std::string_view n);

  // empty implementation for the moment, returns 0
  virtual i64 Inhale (i64 steppe, f64 thyme);
};

}

#endif //ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK
