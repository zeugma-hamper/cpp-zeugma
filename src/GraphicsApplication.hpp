#ifndef PIXELS_DANCE_A_SOFT_RHUMBA
#define PIXELS_DANCE_A_SOFT_RHUMBA

#include <application.hpp>
#include <base_types.hpp>
#include <FrameTime.hpp>
#include <Layer.hpp>
#include <ZeEventMoltoSprinkler.hpp>
#include <ZePublicWaterWorks.hpp>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


namespace charm {

class GraphicsApplication : public Application
{
 public:
  GraphicsApplication ();
  ~GraphicsApplication () override;

  bool StartUp  () override;
  bool Update   () override;
  bool ShutDown () override;

  bool InitWindowingAndGraphics ();
  void ShutDownGraphics ();
  void ShutDownSceneGraph ();
  void Render ();

  void UpdateSceneGraph ();

  MoltoSprinkler &GetSprinkler ();

  void AppendWaterWorks (ZePublicWaterWorks *_pub);
  //removes and deletes water works
  void RemoveWaterWorks (ZePublicWaterWorks *_pub);
  //removes and returns found water works
  ZePublicWaterWorks *ExciseWaterWorks (ZePublicWaterWorks *_pub);

  static FrameTime *GetFrameTime ();

  Layer &GetSceneLayer ();

 protected:

  MoltoSprinkler m_event_sprinkler;
  std::vector<ZePublicWaterWorks *> m_event_drainage;

  GLFWwindow *window;

  Layer *m_scene_graph_layer;
};

}

#endif //PIXELS_DANCE_A_SOFT_RHUMBA
