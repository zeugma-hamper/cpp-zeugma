
#ifndef PIXELS_DANCE_A_SOFT_RHUMBA
#define PIXELS_DANCE_A_SOFT_RHUMBA


#include <application.hpp>
#include <base_types.hpp>
#include <FrameTime.hpp>
#include <Layer.hpp>
#include <ZePublicWaterWorks.hpp>
#include <PlatonicMaes.h>
#include <CMVTrefoil.h>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


namespace charm {


class GraphicsApplication  :  public Application
{
 public:
  GraphicsApplication ();
  ~GraphicsApplication () override;

  CHARM_DELETE_MOVE_COPY(GraphicsApplication);

  bool StartUp     () override;
  bool RunOneCycle () override;
  bool ShutDown    () override;

  bool InitWindowingAndGraphics ();
  void ShutDownGraphics ();
  void ShutDownSceneGraph ();
  void Render ();

  void UpdateSceneGraph (i64 ratch, f64 thyme);
  void UpdateRenderLeaves (i64 ratch, f64 thyme);

  std::size_t NumWaterWorkses ()  const
    { return m_event_drainage . size (); }
  ZePublicWaterWorks *NthWaterWorks (std::size_t ind)
    { return m_event_drainage . at (ind); }

  void AppendWaterWorks (ZePublicWaterWorks *_pub);
  //removes and deletes water works
  void RemoveWaterWorks (ZePublicWaterWorks *_pub);
  //removes and returns found water works
  ZePublicWaterWorks *ExciseWaterWorks (ZePublicWaterWorks *_pub);

  static FrameTime *GetFrameTime ();
  static GraphicsApplication *GetApplication ();

  i32 NumMaeses ()  const;
  PlatonicMaes *NthMaes (i32 ind) const;
  PlatonicMaes *FindMaesByName (std::string_view _name) const;

  PlatonicMaes *ClosestIntersectedMaes (const Vect &frm, const Vect &aim,
                                        Vect *hit_point = NULL);
  i32 NumRenderLeaves ()  const;
  CMVTrefoil *NthRenderLeaf (i32 ind) const;
  CMVTrefoil *FindRenderLeafByName (std::string_view _name) const;

  Layer *GetSceneLayer ();
  Layer *GetNthSceneLayer (i32 nth);
  void AppendSceneLayer (Layer *layer);
  i32 NumSceneLayers () const;

 protected:
  i64 m_global_ratchet;
  std::vector <ZePublicWaterWorks *> m_event_drainage;

  GLFWwindow *m_window;
  std::vector<PlatonicMaes *> m_maes;

  std::vector<Layer *> m_scene_graph_layers;
  std::vector<CMVTrefoil *> m_trefoils;
};


}


#endif //PIXELS_DANCE_A_SOFT_RHUMBA
