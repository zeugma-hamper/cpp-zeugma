#ifndef NODULAR_BITS
#define NODULAR_BITS

#include "base_types.hpp"
#include "SoftValue.hpp"
#include "class_utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace charm
{

class Layer;
class Renderable;

struct TransformComponents
{
  glm::vec3 translation{0.0f};
  glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f};
};

using TransformComponentsSoftValue = AnimSoftValue<TransformComponents, Animation>;

struct Transformation
{
  glm::mat4 model{1.0f};
  glm::mat4 normal{1.0f};
};

using TransformationSoftValue = SoftValue<Transformation>;

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
  void append_child (Node *_node);
  // excise with feeling (deletes)
  void remove_child (Node *_node);
  Node *excise_child (Node *_node);

  // node takes ownership of renderables
  void append_renderable (Renderable *_render);
  //excise with feeling (deletes)
  void remove_renderable (Renderable *_render);
  Renderable *excise_renderable (Renderable *_render);

  void set_layer (Layer *_layer);
  Layer *get_layer () const;

  TransformComponentsSoftValue &get_transform_components_soft ();

  TransformationSoftValue &get_absolute_transformation_soft ();
  glm::mat4 const &get_absolute_model_transformation () const;
  glm::mat4 const &get_absolute_normal_transformation () const;

  TransformationSoftValue &get_transformation_soft ();
  glm::mat4 const &get_model_transformation () const;
  glm::mat4 const &get_normal_transformation () const;

 private:

  Layer *m_layer;
  Node *m_parent;
  std::vector<Node *> m_children;
  std::vector<Renderable *> m_renderables;
  TransformComponentsSoftValue m_tx_components;
  TransformationSoftValue m_tx;
  TransformationSoftValue m_absolute_tx;
};

}

#endif //NODULAR_BITS
