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

  void update_transformations ();
  void update_transformations (TransformationSoftValue const &_parent_tx);

  void enumerate_renderables ();
  void enumerate_renderables (graph_id &_id);

  // node takes ownership of child nodes
  void  append_child (Node *_node);
  // excise with feeling (deletes)
  void  remove_child (Node *_node);
  Node *excise_child (Node *_node);

  // node takes ownership of renderables
  void        append_renderable (Renderable *_render);
  //excise with feeling (deletes)
  void        remove_renderable (Renderable *_render);
  Renderable *excise_renderable (Renderable *_render);

  void   set_layer (Layer *_layer);
  Layer *get_layer () const;

  TransformComponentsSoftValue &get_transform_components_soft ();
  glm::vec3 const &get_translation () const;
  glm::quat const &get_rotation () const;
  glm::vec3 const &get_scale () const;

  TransformationSoftValue &get_absolute_transformation_soft ();
  glm::mat4 const &get_absolute_model_transformation  () const;
  glm::mat4 const &get_absolute_normal_transformation () const;

  TransformationAnimSoftValue &get_transformation_soft ();
  glm::mat4 const &get_model_transformation  () const;
  glm::mat4 const &get_normal_transformation () const;

  void install_component_animation (ComponentAnimation *_animation);
  Animation *get_component_animation () const;

  void install_transform_animation (TransformationAnimation *_animation);
  Animation *get_transformation_animation () const;

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
