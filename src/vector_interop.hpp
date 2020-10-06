#ifndef CROSS_BUT_NOT_THE_USEFUL_ONE
#define CROSS_BUT_NOT_THE_USEFUL_ONE

#include <base_types.hpp>
#include <charm_glm.hpp>

#include "Matrix44.h"

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

inline glm::mat4 as_glm (const Matrix44 &m)
{ return glm::mat4 (m.a_[0],  m.a_[1],  m.a_[2],  m.a_[3],
                    m.a_[4],  m.a_[5],  m.a_[6],  m.a_[7],
                    m.a_[8],  m.a_[9],  m.a_[10], m.a_[11],
                    m.a_[12], m.a_[13], m.a_[14], m.a_[15]);
}


}  // namespace charm runs off the cliff, hangs in the air for a second, then...


#endif //CROSS_BUT_NOT_THE_USEFUL_ONE
