#ifndef THIRD_TIME
#define THIRD_TIME

#include "base_types.hpp"
#include "class_utils.hpp"
#include "Node.hpp"

#include "GeomFumble.h"

#include <charm_glm.hpp>

#include <vector>


namespace charm
{

class Frontier;
class Renderable;
class Node;

class Layer
{
 public:
  friend Node;

  Layer ();
  ~Layer ();

  CHARM_DELETE_COPY  (Layer);
  CHARM_DEFAULT_MOVE (Layer);

  Node *GetRootNode ();

  Frontier *FirstHitFrontier (G::Ray const &_ray, Vect *hit_pt) const;

  std::vector<Renderable *> &GetRenderables ();
  std::vector<Frontier *> &GetFrontiers ();

  bool RenderableToForemost (Renderable *re);
  bool RenderableToRearmost (Renderable *re);

  void SortFrontiers ();

 protected:
  void RemoveFrontier (Frontier *_frontier);

  void RemoveRenderable (Renderable *_rend);
  void RemoveRenderables (std::vector<Renderable *> const &_rends);

  Node m_root_node;
  std::vector<Renderable *> m_renderables;
  std::vector<Frontier *> m_frontiers;
};

}
#endif //THIRD_TIME
