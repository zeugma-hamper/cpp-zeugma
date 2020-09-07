
#ifndef BORING_NAME_MAKE_FUN_LATER
#define BORING_NAME_MAKE_FUN_LATER

#include <string.h>

#define CHARM_DELETE_MOVE(KLASS) \
  KLASS(KLASS &&) = delete; \
  KLASS &operator=(KLASS &&) = delete

#define CHARM_DEFAULT_MOVE(KLASS)  \
  KLASS(KLASS &&) = default; \
  KLASS &operator=(KLASS &&) = default

#define CHARM_DELETE_COPY(KLASS) \
  KLASS(KLASS const &) = delete; \
  KLASS &operator=(KLASS const &) = delete

#define CHARM_DEFAULT_COPY(KLASS)       \
  KLASS(KLASS const &) = default; \
  KLASS &operator=(KLASS const &) = default

#define CHARM_DELETE_MOVE_COPY(KLASS) \
  CHARM_DELETE_MOVE(KLASS); \
  CHARM_DELETE_COPY(KLASS)

#define CHARM_DEFAULT_MOVE_COPY(KLASS) \
  CHARM_DEFAULT_MOVE(KLASS); \
  CHARM_DEFAULT_COPY(KLASS)

namespace charm
{

template<typename T>
void zero_struct (T &_str)
{
  memset (&_str, 0, sizeof (T));
}

}

#endif //BORING_NAME_MAKE_FUN_LATER
