#ifndef SLAPPY_TYPES_LEAVE_A_RED_MARK
#define SLAPPY_TYPES_LEAVE_A_RED_MARK

#include <stddef.h>
#include <stdint.h>

namespace charm
{

  //basic numerics
using i8  = int8_t;
using u8  = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using f32 = float;
using i64 = int64_t;
using u64 = uint64_t;
using f64 = double;
using szt = size_t;

  //
using sort_key = u64;
using graph_id = u64;
using animation_step = u64;

typedef struct {
  union {
    struct { f32 x, y; };
    struct { f32 a0, a1; };
    f32 a[2];
  };
} v2f32;

typedef struct {
  union {
    struct { f32 x, y, z; };
    struct { f32 a0, a1, a2; };
    f32 a[3];
  };
} v3f32;

typedef struct {
  union {
    struct { f32 x, y, z, w; };
    struct { f32 a0, a1, a2, a3; };
    f32 a[4];
  };
} v4f32;



typedef struct {
  union {
    struct { f64 x, y; };
    struct { f64 a0, a1; };
    f64 a[2];
  };
} v2f64;

typedef struct {
  union {
    struct { f64 x, y, z; };
    struct { f64 a0, a1, a2; };
    f64 a[3];
  };
} v3f64;

typedef struct {
  union {
    struct { f64 x, y, z, w; };
    struct { f64 a0, a1, a2, a3; };
    f64 a[4];
  };
} v4f64;

}

#endif
