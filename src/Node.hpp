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
class Frontier;

struct Transformation
{
  glm::mat4 model{1.0f};
  glm::mat4 normal{1.0f};
};

class Node
{
 public:

  Node ();
  virtual ~Node ();

  CHARM_DELETE_COPY (Node);
  CHARM_DEFAULT_MOVE (Node);

  std::array<graph_id, 2> EnumerateGraph (graph_id _base_id = 0, graph_id _base_rend_id = 0);
  void SetGraphID (graph_id _id);
  graph_id GetGraphID () const;

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

  Grappler *Translate (const Vect &tr);
  inline Grappler *Translate (f64 dx, f64 dy, f64 dz)
    { return Translate (Vect (dx, dy, dz)); }

  Grappler *Scale (const Vect &sc);
  inline Grappler *Scale (f64 sx, f64 sy, f64 sz)
    { return Scale (Vect (sx, sy, sz)); }
  inline Grappler *Scale (f64 s)
    { return Scale (s, s, s); }

  Grappler *Rotate (const Vect &ax, f64 an);
  inline Grappler *RotateD (const Vect &ax, f64 an)
    { return Rotate (ax, M_PI / 180.0 * an); }
  Grappler *RotateWithCenter (const Vect &ax, f64 an, const Vect &ce);
  inline Grappler *RotateWithCenterD (const Vect &ax, f64 an, const Vect &ce)
    { return RotateWithCenter (ax, M_PI / 180.0 * an, ce); }

  Grappler *Translate (const ZoftVect &tzo);
  Grappler *Scale (const ZoftVect &szo);
  Grappler *Rotate (const ZoftVect &ax_zft,
                    const ZoftFloat &an_zft,
                    const ZoftVect &ce_zft = ZoftVect (Vect::zerov),
                    const ZoftFloat &ph_zft = ZoftFloat (0.0));
/*
  Grappler *Rotate (ZoftVect &ax_zft,
                    ZoftFloat &an_zft,
                    ZoftVect &ce_zft);
  Grappler *Rotate (ZoftVect &ax_zft,
                    ZoftFloat &an_zft);
*/
  void ClearTransforms ();

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

  void SetFrontier (Frontier *_frontier);
  Frontier *GetFrontier () const;

 private:
  Layer *m_layer;
  Node  *m_parent;
  GrapplerPile *m_graps;
  Frontier *m_frontier;
  graph_id m_id;

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
