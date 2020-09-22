#ifndef BASE_RENDERABLE_DOESNT_DO_MUCH
#define BASE_RENDERABLE_DOESNT_DO_MUCH

#include <base_types.hpp>
#include <class_utils.hpp>

#include <charm-glm.hpp>

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

  // assuming this is going to be updated once softs are installed
  void SetOver (glm::vec4 const &_over);
  glm::vec4 const &GetOver () const;

  // assuming this is going to be updated once softs are installed
  void SetUp (glm::vec4 const &_up);
  glm::vec4 const &GetUp () const;

  virtual void Update ();
  virtual void Draw () = 0;

  bool ShouldDraw () const;

  void SetShouldDraw (bool _tf);

  sort_key GetSortKey () const;
  graph_id GetGraphId () const;

 protected:
  Node *m_node;
  glm::vec4 m_over;
  glm::vec4 m_up;
  sort_key m_sort_key;
  graph_id m_graph_id;
  bool m_should_draw;
};

}

#endif //BASE_RENDERABLE_DOESNT_DO_MUCH
