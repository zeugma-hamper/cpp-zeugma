
#ifndef ZE_UUID_IS_EACH_ONE_THE_ONE_AND_ONLY
#define ZE_UUID_IS_EACH_ONE_THE_ONE_AND_ONLY


#include "base_types.hpp"


namespace charm  {


struct ZeUUID
{ union
    { struct { u8 bytes[16];       };
      struct { u64 a_half, b_half; };
    };

  ZeUUID ();

  bool operator == (const ZeUUID &oth)
    { return (a_half == oth.a_half  &&  b_half == oth.b_half); }

  bool operator != (const ZeUUID &oth)
    { return ! (*this == oth); }
};


//
/// separately, a simple incrementing identifier
//

i64 ZeMonotonicID ();


}


#endif
