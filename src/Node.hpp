
#ifndef NODULAR_BITS
#define NODULAR_BITS


#include <base_types.hpp>
#include <class_utils.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

#include "GrapplerPile.h"


namespace charm  {


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
  explicit Node (Renderable *_renderable);
  virtual ~Node ();

  CHARM_DELETE_COPY (Node);
  CHARM_DELETE_MOVE (Node);

  std::array<graph_id, 2> EnumerateGraph (graph_id _base_id = 0,
                                          graph_id _base_rend_id = 0);
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

  void AppendGrappler (Grappler *g)
    { AssuredGrapplerPile () -> AppendGrappler (g); }
  Grappler *FindGrappler (const std::string &nm);

  Grappler *Translate (const Vect &tr);
  Grappler *Translate (f64 dx, f64 dy, f64 dz)
    { return Translate (Vect (dx, dy, dz)); }

  Grappler *Scale (const Vect &sc);
  Grappler *Scale (f64 sx, f64 sy, f64 sz)
    { return Scale (Vect (sx, sy, sz)); }
  Grappler *Scale (f64 s)
    { return Scale (s, s, s); }

  Grappler *Rotate (const Vect &ax, f64 an);
  Grappler *RotateD (const Vect &ax, f64 an)
    { return Rotate (ax, M_PI / 180.0 * an); }
  Grappler *RotateWithCenter (const Vect &ax, f64 an, const Vect &ce);
  Grappler *RotateWithCenterD (const Vect &ax, f64 an, const Vect &ce)
    { return RotateWithCenter (ax, M_PI / 180.0 * an, ce); }

  Grappler *Translate (ZoftVect &tzo);
  Grappler *Scale (ZoftVect &szo);
  Grappler *Rotate (ZoftVect &ax_zft,
                    ZoftFloat &an_zft,
                    ZoftVect &ce_zft,
                    ZoftFloat &ph_zft);
  Grappler *Rotate (ZoftVect &ax_zft,
                    ZoftFloat &an_zft,
                    ZoftVect &ce_zft)
    { ZoftFloat zf;  return Rotate (ax_zft, an_zft, ce_zft, zf); }
  Grappler *Rotate (ZoftVect &ax_zft,
                    ZoftFloat &an_zft)
    { ZoftVect zv;  ZoftFloat zf;  return Rotate (ax_zft, an_zft, zv, zf); }

  Grappler *Translate (const ZoftVect &tzo);
  Grappler *Scale (const ZoftVect &szo);
  Grappler *Rotate (const ZoftVect &ax_zft,
                    const ZoftFloat &an_zft,
                    const ZoftVect &ce_zft = ZoftVect_zero,
                    const ZoftFloat &ph_zft = ZoftFloat_zero);

  void ClearTransforms ();

  void SetVisibilityForAllLocalRenderables (bool vis);

  Node *Parent ();

  // node takes ownership of child nodes
  void  AppendChild (Node *_node);
  // excise with feeling (deletes)
  void  RemoveChild (Node *_node);
  Node *ExciseChild (Node *_node);

  i64 ChildCount ()  const
    { return m_children . size (); }
  Node *NthChild (i64 ind)
    { return (ind < 0  ||  ind > ChildCount ())  ?  NULL  :  m_children[ind]; }
  std::vector <Node *> ChildListCopy ()
    { return m_children; }

  // node takes ownership of renderables
  void        AppendRenderable (Renderable *_render);
  //excise with feeling (deletes)
  void        RemoveRenderable (Renderable *_render);
  Renderable *ExciseRenderable (Renderable *_render);

  std::vector<Renderable *> &GetRenderables ();
  std::vector<Renderable *> const &GetRenderables () const;

  i64 NumRenderables ()  const;
  Renderable *NthRenderable (i64 ind);

  bool MakeRenderablesForemostInLayer (bool recurse_of_course = true);
  bool MakeRenderablesRearmostInLayer (bool recurse_of_course = true);

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
    m_children[i] -> VisitDepthFirst (_func);
}


}  // unto namespace charm as to veronica cartwright in alien, witches of e'wick


#endif //NODULAR_BITS
