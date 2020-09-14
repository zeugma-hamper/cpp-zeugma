#ifndef UP_AND_DOWN_THE_HIERARCHY
#define UP_AND_DOWN_THE_HIERARCHY

#include "base_types.hpp"
#include "SoftValue.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace charm
{

struct TransformComponents
{
  glm::vec3 translation{0.0f};
  glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f};
};

struct TransformComponentsSoftValue : public AnimSoftValue<TransformComponents, Animation>
{
  void SetTranslation (glm::vec3 const &_translation);
  glm::vec3 const &GetTranslation () const;

  void SetRotation (glm::quat const &_rot);
  glm::quat const &GetRotation () const;

  void SetScale (f32 _scale);
  void SetScale (glm::vec3 const &_scale);
  glm::vec3 const &GetScale () const;
};

struct Transformation
{
  glm::mat4 model{1.0f};
  glm::mat4 normal{1.0f};
};

struct TransformationSoftValue : public SoftValue<Transformation>
{
  void SetModel (glm::mat4 const &_mod);
  glm::mat4 const &GetModel () const;

  void SetNormal (glm::mat4 const &_norm);
  glm::mat4 const &GetNormal () const;
};

struct TransformationAnimSoftValue : public AnimSoftValue<Transformation, Animation>
{
  void SetModel (glm::mat4 const &_mod);
  glm::mat4 const &GetModel () const;

  void SetNormal (glm::mat4 const &_norm);
  glm::mat4 const &GetNormal () const;
};

}

#endif //UP_AND_DOWN_THE_HIERARCHY
