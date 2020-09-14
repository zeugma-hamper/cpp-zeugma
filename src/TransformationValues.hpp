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
  void set_translation (glm::vec3 const &_translation);
  glm::vec3 const &get_translation () const;

  void set_rotation (glm::quat const &_rot);
  glm::quat const &get_rotation () const;

  void set_scale (f32 _scale);
  void set_scale (glm::vec3 const &_scale);
  glm::vec3 const &get_scale () const;
};

struct Transformation
{
  glm::mat4 model{1.0f};
  glm::mat4 normal{1.0f};
};

struct TransformationSoftValue : public SoftValue<Transformation>
{
  void set_model (glm::mat4 const &_mod);
  glm::mat4 const &get_model () const;

  void set_normal (glm::mat4 const &_norm);
  glm::mat4 const &get_normal () const;
};

struct TransformationAnimSoftValue : public AnimSoftValue<Transformation, Animation>
{
  void set_model (glm::mat4 const &_mod);
  glm::mat4 const &get_model () const;

  void set_normal (glm::mat4 const &_norm);
  glm::mat4 const &get_normal () const;
};

}

#endif //UP_AND_DOWN_THE_HIERARCHY
