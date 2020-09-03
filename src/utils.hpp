
#ifndef BORING_NAME_MAKE_FUN_LATER
#define BORING_NAME_MAKE_FUN_LATER

#include <string.h>

namespace charm
{

template<typename T>
void zero_struct (T &_str)
{
  memset (&_str, 0, sizeof (T));
}

}

#endif //BORING_NAME_MAKE_FUN_LATER
