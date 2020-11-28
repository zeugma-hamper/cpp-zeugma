
#ifndef BASE_RENDERABLE_DOESNT_DO_MUCH
#define BASE_RENDERABLE_DOESNT_DO_MUCH


#include <base_types.hpp>
#include <class_utils.hpp>

#include <charm_glm.hpp>

#include "ZoftThing.h"


namespace charm {


class Node;


class Renderable
{
 public:
  friend Node;

  Renderable ();
  Renderable (Node *_node);
  virtual ~Renderable ();

  CHARM_DELETE_MOVE_COPY (Renderable);

  const Vect &Over ()  const;
  const Vect &Up ()  const;
  const ZeColor &AdjColor ()  const;

  void SetOver (const Vect &o);
  void SetUp (const Vect &u);
  void SetAdjColor (const ZeColor &c);

  ZoftVect &OverZoft ();
  ZoftVect &UpZoft ();
  ZoftColor &AdjColorZoft ();

  virtual void Update ();
  virtual void Draw (u16 vyu_id) = 0;

  bool ShouldDraw () const;
  void SetShouldDraw (bool _tf);

  // bool MakeForemostInLayer ();
  // bool MakeRearmostInLayer ();

  sort_key GetSortKey () const;

  graph_id GetGraphId () const;
  void SetGraphID (graph_id _id);

 protected:
  Node *m_node;
  ZoftVect m_over;
  ZoftVect m_up;
  ZoftColor m_adjc;
  sort_key m_sort_key;
  graph_id m_graph_id;
  bool m_should_draw;
};


}


#endif //BASE_RENDERABLE_DOESNT_DO_MUCH
