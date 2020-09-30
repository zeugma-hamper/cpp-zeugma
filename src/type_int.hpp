#ifndef THIS_IS_INTERESTING
#define THIS_IS_INTERESTING

#include <base_types.hpp>

namespace charm
{

//this is safe with clang and g++, would need to test with MSVC

template<typename T>
struct counter
{
  static T next ()
  {
    static T now = 0;
    return ++now;
  }
};

template<typename T>
struct index
{
  static u32 get ()
  {
    static u32 ind = counter<u32>::next ();
    szt const size = sizeof (T); // assure complete type
    (void)size;
    return ind;
  }
};

}

#endif // THIS_IS_INTERESTING
