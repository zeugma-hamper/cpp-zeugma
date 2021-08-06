
#include "ZeUUID.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


namespace zeugma  {


ZeUUID::ZeUUID ()
{ boost::uuids::uuid yooyoo = boost::uuids::random_generator()();
  u8 *pee = &bytes[0];
  auto omega = yooyoo . end ();
  for (auto it = yooyoo . begin ()  ;  it != omega  ;  ++it, ++pee)
    *pee = *it;
}


//
/// not uuid hereinafter
//

static i64 _ze_monotonic_id = 6;

i64 ZeMonotonicID ()
{ return ++_ze_monotonic_id; }


}  // willard's gone upriver to dispatch colonel namspace zeugma
