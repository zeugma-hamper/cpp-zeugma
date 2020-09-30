#ifndef CROSS_BUT_NOT_THE_USEFUL_ONE
#define CROSS_BUT_NOT_THE_USEFUL_ONE

#include <base_types.hpp>
#include <charm_glm.hpp>

namespace charm
{

inline glm::vec3 as_glm (v3f32 const &_v)
{
  return {_v.x, _v.y, _v.z};
}

inline glm::vec3 as_glm (v3f64 const &_v)
{
  return {_v.x, _v.y, _v.z};
}

inline glm::vec4 as_glm (v4f32 const &_v)
{
  return {_v.x, _v.y, _v.z, _v.w};
}

inline glm::vec4 as_glm (v4f64 const &_v)
{
  return {_v.x, _v.y, _v.z, _v.w};
}

}

#endif //CROSS_BUT_NOT_THE_USEFUL_ONE
