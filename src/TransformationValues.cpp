#include "TransformationValues.hpp"

namespace charm
{

void TransformComponentsSoftValue::SetTranslation (glm::vec3 const &_translation)
{
  SetDirty (true);
  m_value.translation = _translation;
}

glm::vec3 const &TransformComponentsSoftValue::GetTranslation () const
{
  return m_value.translation;
}

void TransformComponentsSoftValue::SetRotation (glm::quat const &_rot)
{
  SetDirty (true);
  m_value.rotation = _rot;
}

glm::quat const &TransformComponentsSoftValue::GetRotation () const
{
  return m_value.rotation;
}

void TransformComponentsSoftValue::SetScale (f32 _scale)
{
  SetDirty (true);
  m_value.scale = glm::vec3 (_scale);
}

void TransformComponentsSoftValue::SetScale (glm::vec3 const &_scale)
{
  SetDirty (true);
  m_value.scale = _scale;
}

glm::vec3 const &TransformComponentsSoftValue::GetScale () const
{
  return m_value.scale;
}

void TransformationSoftValue::SetModel (glm::mat4 const &_mod)
{
  SetDirty (true);
  m_value.model = _mod;
}

glm::mat4 const &TransformationSoftValue::GetModel () const
{
  return m_value.model;
}

void TransformationSoftValue::SetNormal (glm::mat4 const &_norm)
{
  SetDirty (true);
  m_value.normal = _norm;
}

glm::mat4 const &TransformationSoftValue::GetNormal () const
{
  return m_value.normal;
}

void TransformationAnimSoftValue::SetModel (glm::mat4 const &_mod)
{
  SetDirty (true);
  m_value.model = _mod;
}

glm::mat4 const &TransformationAnimSoftValue::GetModel () const
{
  return m_value.model;
}

void TransformationAnimSoftValue::SetNormal (glm::mat4 const &_norm)
{
  SetDirty (true);
  m_value.normal = _norm;
}

glm::mat4 const &TransformationAnimSoftValue::GetNormal () const
{
  return m_value.normal;
}

}
