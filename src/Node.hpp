#ifndef NODULAR_BITS
#define NODULAR_BITS

#include <base_types.hpp>
#include <class_utils.hpp>
#include <Animation.hpp>
#include <SoftValue.hpp>
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

  TransformComponentsSoftValue &GetTransformComponentsSoft ();
  glm::vec3 const &GetTranslation () const;
  glm::quat const &GetRotation () const;
  glm::vec3 const &GetScale () const;

  TransformationSoftValue &GetAbsoluteTransformationSoft ();
  glm::mat4 const &GetAbsoluteModelTransformation  () const;
  glm::mat4 const &GetAbsoluteNormalTransformation () const;

  TransformationAnimSoftValue &GetTransformationSoft ();
  glm::mat4 const &GetModelTransformation  () const;
  glm::mat4 const &GetNormalTransformation () const;

  void InstallComponentAnimation (ComponentAnimation *_animation);
  Animation *GetComponentAnimation () const;

  void InstallTransformAnimation (TransformationAnimation *_animation);
  Animation *GetTransformationAnimation () const;

 private:
  Layer *m_layer;
  Node  *m_parent;

  std::vector<Node *>       m_children;
  std::vector<Renderable *> m_renderables;

  TransformComponentsSoftValue m_tx_components;
  TransformationAnimSoftValue  m_tx;
  TransformationSoftValue      m_absolute_tx;
};

}

#endif //NODULAR_BITS
