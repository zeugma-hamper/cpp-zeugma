#ifndef BASE_RENDERABLE_DOESNT_DO_MUCH
#define BASE_RENDERABLE_DOESNT_DO_MUCH

#include <base_types.hpp>
#include <class_utils.hpp>

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

  virtual void Update ();
  virtual void Draw () = 0;

  bool ShouldDraw () const;

  void SetShouldDraw (bool _tf);

  sort_key GetSortKey () const;
  graph_id GetGraphId () const;

 protected:
  Node *m_node;
  sort_key m_sort_key;
  graph_id m_graph_id;
  bool m_should_draw;
};

}

#endif //BASE_RENDERABLE_DOESNT_DO_MUCH
