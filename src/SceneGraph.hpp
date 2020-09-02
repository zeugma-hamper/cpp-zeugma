
#ifndef THIRD_TIME
#define THIRD_TIME

#include "base_types.hpp"
#include "SoftValue.hpp"
#include "AnimationSystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <vector>

#define CHARM_DELETE_MOVE(KLASS) \
  KLASS(KLASS &&) = delete; \
  KLASS &operator=(KLASS &&) = delete

#define CHARM_DEFAULT_MOVE(KLASS)  \
  KLASS(KLASS &&) = default; \
  KLASS &operator=(KLASS &&) = default

#define CHARM_DELETE_COPY(KLASS) \
  KLASS(KLASS const &) = delete; \
  KLASS &operator=(KLASS const &) = delete

#define CHARM_DEFAULT_COPY(KLASS)       \
  KLASS(KLASS const &) = default; \
  KLASS &operator=(KLASS const &) = default

#define CHARM_DELETE_MOVE_COPY(KLASS) \
  CHARM_DELETE_MOVE(KLASS); \
  CHARM_DELETE_COPY(KLASS)

#define CHARM_DEFAULT_MOVE_COPY(KLASS) \
  CHARM_DEFAULT_MOVE(KLASS); \
  CHARM_DEFAULT_COPY(KLASS)

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
  virtual ~Renderable ();

  CHARM_DELETE_MOVE_COPY (Renderable);

  virtual void update (Node *node);
  virtual void draw (Node *node) = 0;

  bool should_draw () const;

  void set_should_draw (bool _tf);

  sort_key get_sort_key () const;
  graph_id get_graph_id () const;

 protected:
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

  CHARM_DELETE_COPY (Layer);
  CHARM_DEFAULT_MOVE (Layer);

  Node *root_node ();

  std::vector<Renderable *> &get_renderables ();

 protected:

  void remove_renderables (std::vector<Renderable *> const &_rends);

  Node m_root_node;
  std::vector<Renderable *> m_renderables;
};

}
#endif //THIRD_TIME
