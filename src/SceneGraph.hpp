
#ifndef THIRD_TIME
#define THIRD_TIME

#include "base_types.hpp"
#include "SoftValue.hpp"
#include "AnimationSystem.hpp"
#include "utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <vector>


namespace charm
{

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

class Renderable;
class Node;
class Layer;

class Renderable
{
 public:
  friend Node;

  using sort_key = std::uint64_t;
  using graph_id = std::uint64_t;

  Renderable ();
  Renderable (Node *_node);
  virtual ~Renderable ();

  CHARM_DELETE_MOVE_COPY (Renderable);

  virtual void update ();
  virtual void draw () = 0;

  bool should_draw () const;

  void set_should_draw (bool _tf);

  sort_key get_sort_key () const;
  graph_id get_graph_id () const;

 protected:
  Node *m_node;
  sort_key m_sort_key;
  graph_id m_graph_id;
  bool m_should_draw;
};

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
  void enumerate_renderables (Renderable::graph_id &_id);

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

class Layer
{
 public:
  friend Node;

  Layer ();
  ~Layer ();

  CHARM_DELETE_COPY  (Layer);
  CHARM_DEFAULT_MOVE (Layer);

  Node *root_node ();

  std::vector<Renderable *> &get_renderables ();

  glm::mat4 const &get_projection_matrix () const;
  void set_projection_matrix (glm::mat4 const &_proj);

  glm::mat4 const &get_camera_matrix () const;
  void set_camera_matrix (glm::mat4 const &_cam);

 protected:

  void remove_renderables (std::vector<Renderable *> const &_rends);

  Node m_root_node;
  std::vector<Renderable *> m_renderables;
  glm::mat4 m_projection_matrix;
  glm::mat4 m_camera_matrix;
};

}
#endif //THIRD_TIME
