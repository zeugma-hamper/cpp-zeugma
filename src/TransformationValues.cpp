#include "TransformationValues.hpp"

namespace charm
{
  void TransformComponentsSoftValue::set_translation (glm::vec3 const &_translation)
{
  set_dirty (true);
  m_value.translation = _translation;
}

glm::vec3 const &TransformComponentsSoftValue::get_translation () const
{
  return m_value.translation;
}

void TransformComponentsSoftValue::set_rotation (glm::quat const &_rot)
{
  set_dirty (true);
  m_value.rotation = _rot;
}

glm::quat const &TransformComponentsSoftValue::get_rotation () const
{
  return m_value.rotation;
}

void TransformComponentsSoftValue::set_scale (f32 _scale)
{
  set_dirty (true);
  m_value.scale = glm::vec3 (_scale);
}

void TransformComponentsSoftValue::set_scale (glm::vec3 const &_scale)
{
  set_dirty (true);
  m_value.scale = _scale;
}

glm::vec3 const &TransformComponentsSoftValue::get_scale () const
{
  return m_value.scale;
}

void TransformationSoftValue::set_model (glm::mat4 const &_mod)
{
  set_dirty (true);
  m_value.model = _mod;
}

glm::mat4 const &TransformationSoftValue::get_model () const
{
  return m_value.model;
}

void TransformationSoftValue::set_normal (glm::mat4 const &_norm)
{
  set_dirty (true);
  m_value.normal = _norm;
}

glm::mat4 const &TransformationSoftValue::get_normal () const
{
  return m_value.normal;
}

void TransformationAnimSoftValue::set_model (glm::mat4 const &_mod)
{
  set_dirty (true);
  m_value.model = _mod;
}

glm::mat4 const &TransformationAnimSoftValue::get_model () const
{
  return m_value.model;
}

void TransformationAnimSoftValue::set_normal (glm::mat4 const &_norm)
{
  set_dirty (true);
  m_value.normal = _norm;
}

glm::mat4 const &TransformationAnimSoftValue::get_normal () const
{
  return m_value.normal;
}

}
