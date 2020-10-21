#ifndef THIS_IS_INTERESTING
#define THIS_IS_INTERESTING

#include <base_types.hpp>

namespace charm
{

//this is safe with clang and g++, would need to test with MSVC
//references of references:
//https://stackoverflow.com/questions/52851239/local-static-variable-linkage-in-a-template-class-static-member-function
//https://stackoverflow.com/questions/32172137/local-static-thread-local-variables-of-inline-functions
template<typename T>
struct ch_counter
{
  static T next ()
  {
    static T now = 0;
    return ++now;
  }
};

template<typename T>
struct ch_index
{
  static u32 get ()
  {
    static u32 ind = ch_counter<u32>::next ();
    szt const size = sizeof (T); // assure complete type
    (void)size;
    return ind;
  }
};

}

#endif // THIS_IS_INTERESTING
