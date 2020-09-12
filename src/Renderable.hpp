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

}
