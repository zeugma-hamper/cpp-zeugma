#ifndef THIRD_TIME
#define THIRD_TIME

#include "base_types.hpp"
#include "SoftValue.hpp"
#include "AnimationSystem.hpp"
#include "class_utils.hpp"
#include "Node.hpp"

#include <glm/glm.hpp>

#include <vector>


namespace charm
{

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

  Node *root_node ();

  std::vector<Renderable *> &get_renderables ();

  glm::mat4 const &get_projection_matrix () const;
  void set_projection_matrix (glm::mat4 const &_proj);

  glm::mat4 const &get_camera_matrix () const;
  void set_camera_matrix (glm::mat4 const &_cam);

 protected:

  void remove_renderable (Renderable *_rend);
  void remove_renderables (std::vector<Renderable *> const &_rends);

  Node m_root_node;
  std::vector<Renderable *> m_renderables;
  glm::mat4 m_projection_matrix;
  glm::mat4 m_camera_matrix;
};

}
#endif //THIRD_TIME
