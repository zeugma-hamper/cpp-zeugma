#ifndef NODULAR_BITS
#define NODULAR_BITS

#include <base_types.hpp>
#include <class_utils.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace charm
{

class Layer;
class Renderable;

struct Transformation
{
  glm::mat4 model{1.0f};
  glm::mat4 normal{1.0f};
};

class Node
{
 public:

  Node ();
  ~Node ();

  CHARM_DELETE_COPY (Node);
  CHARM_DEFAULT_MOVE (Node);

  void EnumerateRenderables ();
  void EnumerateRenderables (graph_id &_id);

  void UpdateTransformsHierarchically ();
  void UpdateTransformsHierarchically (Transformation const &_parent, bool _is_dirty);

  Transformation const &GetAbsoluteTransformation () const;

  void SetLocalTransformation (Transformation const &_local);
  void SetLocalTransformation (glm::mat4 const &_vertex_tx);
  void SetLocalTransformation (glm::mat4 const &_vertex_tx,
                               glm::mat4 const &_normal_tx);

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


 private:
  Layer *m_layer;
  Node  *m_parent;

  std::vector<Node *>       m_children;
  std::vector<Renderable *> m_renderables;

  Transformation m_local_tx;
  Transformation m_absolute_tx;
  bool m_local_tx_dirty_flag;
};

}

#endif //NODULAR_BITS
