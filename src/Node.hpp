#ifndef NODULAR_BITS
#define NODULAR_BITS

#include <base_types.hpp>
#include <class_utils.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

#include "GrapplerPile.h"

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

  void UpdateTransformsHierarchically (i64 ratch, f64 thyme);
  void UpdateTransformsHierarchically (Transformation const &_parent,
                                       bool _is_dirty, i64 ratch, f64 thyme);

  // callable with one parameter 'Node &'
  template<typename Functor>
  void VisitDepthFirst (Functor &&_func);

  Transformation const &GetAbsoluteTransformation () const;

  void SetLocalTransformation (Transformation const &_local);
  void SetLocalTransformation (glm::mat4 const &_vertex_tx);
  void SetLocalTransformation (glm::mat4 const &_vertex_tx,
                               glm::mat4 const &_normal_tx);

  GrapplerPile *AssuredGrapplerPile ();
  GrapplerPile *UnsecuredGrapplerPile ();

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

  std::vector<Renderable *> &GetRenderables ();
  std::vector<Renderable *> const &GetRenderables () const;

  void   SetLayer (Layer *_layer);
  Layer *GetLayer () const;


 private:
  Layer *m_layer;
  Node  *m_parent;
  GrapplerPile *m_graps;

  std::vector<Node *>       m_children;
  std::vector<Renderable *> m_renderables;

  Transformation m_local_tx;
  Transformation m_absolute_tx;
  bool m_local_tx_dirty_flag;
};

template<typename Functor>
void Node::VisitDepthFirst (Functor &&_func)
{
  _func (*this);

  szt const count = m_children.size ();
  for (szt i = 0; i < count; ++i)
    m_children[i]->VisitDepthFirst (_func);
}

}

#endif //NODULAR_BITS
