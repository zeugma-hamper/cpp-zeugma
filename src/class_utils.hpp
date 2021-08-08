
//
// (c) treadle & loam, provisioners llc
//

#ifndef BORING_NAME_MAKE_FUN_LATER
#define BORING_NAME_MAKE_FUN_LATER


#include <base_types.hpp>

#include <string.h>

#define ZEUGMA_DELETE_MOVE(KLASS) \
  KLASS(KLASS &&) = delete; \
  KLASS &operator=(KLASS &&) = delete

#define ZEUGMA_DEFAULT_MOVE(KLASS)  \
  KLASS(KLASS &&) = default; \
  KLASS &operator=(KLASS &&) = default

#define ZEUGMA_DELETE_COPY(KLASS) \
  KLASS(KLASS const &) = delete; \
  KLASS &operator=(KLASS const &) = delete

#define ZEUGMA_DEFAULT_COPY(KLASS)       \
  KLASS(KLASS const &) = default; \
  KLASS &operator=(KLASS const &) = default

#define ZEUGMA_DELETE_MOVE_COPY(KLASS) \
  ZEUGMA_DELETE_MOVE(KLASS); \
  ZEUGMA_DELETE_COPY(KLASS)

#define ZEUGMA_DEFAULT_MOVE_COPY(KLASS) \
  ZEUGMA_DEFAULT_MOVE(KLASS); \
  ZEUGMA_DEFAULT_COPY(KLASS)


namespace zeugma
{


template<typename T>
void ZeroStruct (T &_str)
{
  memset (&_str, 0, sizeof (T));
}


}


#endif //BORING_NAME_MAKE_FUN_LATER
