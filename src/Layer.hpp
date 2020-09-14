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

  Node *GetRootNode ();

  std::vector<Renderable *> &GetRenderables ();

  glm::mat4 const &GetProjectionMatrix () const;
  void SetProjectionMatrix (glm::mat4 const &_proj);

  glm::mat4 const &GetCameraMatrix () const;
  void SetCameraMatrix (glm::mat4 const &_cam);

 protected:

  void RemoveRenderable (Renderable *_rend);
  void RemoveRenderables (std::vector<Renderable *> const &_rends);

  Node m_root_node;
  std::vector<Renderable *> m_renderables;
  glm::mat4 m_projection_matrix;
  glm::mat4 m_camera_matrix;
};

}
#endif //THIRD_TIME
