#ifndef NODULAR_BITS
#define NODULAR_BITS

#include <base_types.hpp>
#include <class_utils.hpp>
#include <Animation.hpp>
#include <SoftValue.hpp>
#include <SoftTypes.hpp>
#include <TransformationValues.hpp>

#include <vector>

namespace charm
{

class Layer;
class Renderable;

using ComponentAnimation = SoftAnimation<TransformComponentsSoftValue>;
using TransformationAnimation = SoftAnimation<TransformationAnimSoftValue>;

class Node
{
 public:

  Node ();
  ~Node ();

  CHARM_DELETE_COPY (Node);
  CHARM_DEFAULT_MOVE (Node);

  void UpdateTransformations ();
  void UpdateTransformations (TransformationSoftValue const &_parent_tx);

  void EnumerateRenderables ();
  void EnumerateRenderables (graph_id &_id);

  // node takes ownership of child nodes
  void  AppendChild (Node *_node);
  // excise with feeling (deletes)
  void  RemoveChild (Node *_node);
  Node *ExciseChild (Node *_node);

  // node takes ownership of renderables
  void        AppendRenderable (Renderable *_render);
  //excise with feeling (deletes)
  void        RemoveRenderable (Renderable *_render);
  Renderable *ExciseRenderable (Renderable *_render);

  void   SetLayer (Layer *_layer);
  Layer *GetLayer () const;

  VecAnim &GetTranslationSoft ();
  glm::vec3 const &GetTranslation () const;
  void InstallTranslationAnimation (SoftAnimation<VecAnim> *_anim);

  QuatAnim &GetRotationSoft ();
  glm::quat const &GetRotation () const;
  void InstallRotationAnimation (SoftAnimation<QuatAnim> *_anim);

  VecAnim &GetScaleSoft ();
  glm::vec3 const &GetScale () const;
  void InstallScaleAnimation (SoftAnimation<VecAnim> *_anim);

  TransformationAnimSoftValue &GetTransformationSoft ();
  glm::mat4 const &GetModelTransformation  () const;
  glm::mat4 const &GetNormalTransformation () const;

  TransformationSoftValue &GetAbsoluteTransformationSoft ();
  glm::mat4 const &GetAbsoluteModelTransformation  () const;
  glm::mat4 const &GetAbsoluteNormalTransformation () const;

  void InstallTransformAnimation (TransformationAnimation *_animation);
  Animation *GetTransformationAnimation () const;

 private:
  Layer *m_layer;
  Node  *m_parent;

  std::vector<Node *>       m_children;
  std::vector<Renderable *> m_renderables;

  VecAnim                     m_translation;
  VecAnim                     m_scale;
  QuatAnim                    m_rotation;
  TransformationAnimSoftValue m_tx;
  TransformationSoftValue     m_absolute_tx;
};

}

#endif //NODULAR_BITS
