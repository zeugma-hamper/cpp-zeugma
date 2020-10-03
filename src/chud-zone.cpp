#include <application.hpp>
#include <base_types.hpp>
#include <class_utils.hpp>

#include <DecodePipeline.hpp>
#include <FrameTime.hpp>
#include <Layer.hpp>
#include <Node.hpp>
#include <PipelineTerminus.hpp>
#include <Renderable.hpp>
#include <RectangleRenderable.hpp>
#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>
#include <VideoSystem.hpp>

#include "ZoftThing.h"
#include "LoopZoft.h"
#include "InterpZoft.h"
#include "SinuZoft.h"
#include "RoGrappler.h"
#include "ScGrappler.h"
#include "TrGrappler.h"

#include "Bolex.h"

#include "vector_interop.hpp"

#include <bgfx_utils.hpp>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <charm_glm.hpp>

#include <string_view>

#include <stdio.h>

#include <Matte.hpp>

using namespace charm;

class dead_zone final : public charm::Application
{
 public:
  dead_zone ();
  ~dead_zone () override final;

  bool StartUp     () override final;
  bool RunOneCycle () override final;
  bool ShutDown    () override final;

  bool DoWhatThouWilt (i64 ratch, f64 thyme)  override;

  bool InitWindowingAndGraphics ();
  void ShutDownGraphics ();
  void ShutDownSceneGraph ();
  void Render ();

  void UpdateSceneGraph (i64 ratch, f64 thyme);

  static FrameTime *GetFrameTime ();

  Layer &GetSceneLayer ();

 protected:

  Bolex *cam;

  GLFWwindow *window;

  Layer *m_scene_graph_layer;
};



#define ERROR_RETURN_VAL(MSG, VAL)                 \
  {                                                \
    fprintf (stderr, "%s\n", MSG);                 \
    return VAL;                                    \
  }

#define ERROR_RETURN(MSG)                          \
   {                                               \
    fprintf (stderr, "%s\n", MSG);                 \
    return;                                        \
  }


static void glfw_error_callback (int, const char *_msg)
{
  Application::StopRunning();

  fprintf (stderr, "glfw error: %s\n", _msg);
}

//void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
static void glfw_key_callback(GLFWwindow *window, int key, int, int action, int)
{
  if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
      && action == GLFW_RELEASE)
    {
      glfwSetWindowShouldClose (window, GLFW_TRUE);
      Application::StopRunning();
    }
}

bool dead_zone::InitWindowingAndGraphics ()
{
  glfwSetErrorCallback(glfw_error_callback);
  if (! glfwInit())
    ERROR_RETURN_VAL ("error initializing glfw", false);

  glfwWindowHint (GLFW_RED_BITS, 8);
  glfwWindowHint (GLFW_GREEN_BITS, 8);
  glfwWindowHint (GLFW_BLUE_BITS, 8);
  glfwWindowHint (GLFW_DEPTH_BITS, 24);
  glfwWindowHint (GLFW_STENCIL_BITS, 8);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow (1920, 1080, "dead zone", nullptr, nullptr);
  if (! window)
    ERROR_RETURN_VAL ("couldn't create window", false);

  glfwSetKeyCallback(window, glfw_key_callback);

  bgfx::renderFrame();
  bgfx::Init init;

  init.type = bgfx::RendererType::OpenGL;
  init.platformData.ndt = glfwGetX11Display();
  init.platformData.nwh = (void *)glfwGetX11Window(window);
  init.platformData.context = glfwGetGLXContext(window);
  int glfw_width, glfw_height;
  glfwGetWindowSize(window, &glfw_width, &glfw_height);
  init.resolution.width = glfw_width;
  init.resolution.height = glfw_height;
  init.resolution.reset = BGFX_RESET_VSYNC;
  init.resolution.numBackBuffers = 1;

  if (! bgfx::init (init))
    ERROR_RETURN_VAL ("couldn't initialize bgfx", false);

  bgfx::setViewRect(0, 0, 0, glfw_width, glfw_height);
  bgfx::setViewScissor(0);
  bgfx::setViewClear (0, BGFX_CLEAR_COLOR, 0x808080FF);
  // use depth integer to order GPU draw submission
  // drawables are enumerated by order in scene graph,
  // so that id number is used as the depth.
  bgfx::setViewMode (0, bgfx::ViewMode::DepthAscending);

  return true;
}

void dead_zone::ShutDownGraphics ()
{
  bgfx::shutdown();
  glfwTerminate();
}

void dead_zone::Render ()
{
  for (Renderable *r : m_scene_graph_layer->GetRenderables())
    r->Update ();

  bgfx::touch (0);

  glm::mat4 view_transform = glm::lookAt (as_glm (cam -> ViewLoc ()),
                                          as_glm (cam -> ViewCOI ()),
                                          as_glm (cam -> ViewUp ()));

  glm::mat4 proj_transform
    = glm::perspective ((float) cam -> ViewVertAngleD (),
                        (float) (sin (0.5 * cam -> ViewHorizAngle ())
                                 / sin (0.5 * cam -> ViewVertAngle ())),
                        (float) cam -> NearClipDist (),
                        (float) cam -> FarClipDist ());

  bgfx::setViewTransform(0, glm::value_ptr (view_transform),
                         glm::value_ptr (proj_transform));

  for (Renderable *r : m_scene_graph_layer->GetRenderables())
    r->Draw();

  bgfx::frame ();
}

FrameTime *s_dead_zone_frame_time{nullptr};

dead_zone::dead_zone ()
  : window {nullptr},
    m_scene_graph_layer {new Layer}
{ cam = new Bolex;
  cam -> SetViewLoc (Vect (0.0, 0.0, 10.0))
    . SetViewCOI (Vect (0.0, 0.0, 2.0))
    . SetViewUp (Vect (0.0, 1.0, 0.0))
    . SetViewDist (8.0)
    . SetProjectionType (Bolex::ProjType::PERSPECTIVE)
    . SetViewVertAngleD (47.0)
    . SetViewHorizAngleD (90.0)
    . SetNearClipDist (0.005)  .  SetFarClipDist (20.0);

  SinuVect sv (Vect (0.0, 0.0, 0.175), 1.0, Vect (0.0, 0.0, 10.0));
//  cam -> ViewLocZoft () . BecomeLike (sv);
}

dead_zone::~dead_zone ()
{
}

bool dead_zone::StartUp ()
{
  s_dead_zone_frame_time = new FrameTime;
  bool ret = InitWindowingAndGraphics();
  VideoSystem::Initialize();

  return ret;
}

Node *s_nodal = nullptr;
Node *dr_no = nullptr;

static i64 global_ratchet = 0;

bool dead_zone::RunOneCycle ()
{
  GetFrameTime()->UpdateTime();
  f64 global_frame_thyme = GetFrameTime () -> GetCurrentTime ();

  global_ratchet += 8;

  glfwPollEvents();

  if (ProtoZoftThingGuts::IsMassBreathing ())
    ProtoZoftThingGuts::MassBreather ()
      -> Inhale (global_ratchet, global_frame_thyme);

  DoWhatThouWilt (global_ratchet, global_frame_thyme);

  VideoSystem *video_system = VideoSystem::GetSystem ();
  video_system->PollMessages();
  video_system->UploadFrames();

  UpdateSceneGraph (global_ratchet, global_frame_thyme);

  cam -> Inhale (global_ratchet, global_frame_thyme);

  Render ();

  return true;
}


LoopFloat elzyeff { 22.2, 3.3, 4.5 };
LoopVect elzyvee { Vect (0.0, 1.0, 2.0), Vect (1.0, -1.0, 3.0), 5.5 };
InterpFloat ayezee { -1.0, 1.0, 4.7 };

bool dead_zone::DoWhatThouWilt (i64 ratch, f64 thyme)
{ if (ayezee.val > 0.993  &&  ayezee.val < 0.999)
    elzyvee . Restart ();

  cam -> ViewLoc () . SpewToStderr ();
  fprintf (stderr, " is cammy's loc, and interp is all <%.2lf>\n",
           ayezee.val);

  return true;
}


void dead_zone::UpdateSceneGraph(i64 ratch, f64 thyme)
{
  m_scene_graph_layer->GetRootNode()
    -> UpdateTransformsHierarchically (ratch, thyme);
  m_scene_graph_layer->GetRootNode()->EnumerateRenderables();
}

void dead_zone::ShutDownSceneGraph()
{
  delete m_scene_graph_layer;
  m_scene_graph_layer = nullptr;
}

bool dead_zone::ShutDown ()
{
  ShutDownSceneGraph ();
  VideoSystem::ShutDown();
  ShutDownGraphics ();

  delete s_dead_zone_frame_time;
  s_dead_zone_frame_time = nullptr;

  return true;
}

FrameTime *dead_zone::GetFrameTime ()
{
  return s_dead_zone_frame_time;
}

Layer &dead_zone::GetSceneLayer ()
{
  return *m_scene_graph_layer;
}

int main (int, char **)
{
  dead_zone zone;
  if (! zone.StartUp ())
    return -1;

  Layer &layer = zone.GetSceneLayer();

  s_nodal = new Node ();

  s_nodal->SetLocalTransformation(glm::translate(glm::vec3{0.0f, 0.0f, 9.0f})
                                  * glm::scale (glm::vec3 {10.0f}));

  std::vector<FilmInfo> configs = ReadFilmInfo ("../jh-film-config.toml");
  assert (configs.size () > 0);

  FilmInfo &film_info = configs[0];
  assert (film_info.clips.size () > 0);
  // ClipInfo &clip_info = film_info.clips[0];

  // std::string file
  //   = "file:///home/blake/tlp/tamper-blu-mkv/the-fall-blu.mov";

  std::string uri = std::string ("file://") + film_info.film_path.c_str ();
  // MattedVideoRenderable *matte_renderable
  //   = new MattedVideoRenderable (uri,
  //                                clip_info.start_time,
  //                                clip_info.start_time + clip_info.duration,
  //                                clip_info.directory);

  RectangleRenderable *rect_rend = new RectangleRenderable ();
  (dr_no = new Node) -> AppendRenderable (rect_rend);
  layer . GetRootNode () -> AppendChild (dr_no);

  VideoRenderable *renderable
    = new VideoRenderable (uri);

  s_nodal->AppendRenderable(renderable);
  layer.GetRootNode()->AppendChild(s_nodal);
//  SinuVect sv (Vect (0.1), 1.0, Vect (10.0));
  SinuVect sv (Vect (5.0, 0.0, 0.0), 1.0, Vect (0.0, 0.0, 9.0));
  SinuFloat sf (0.5, 0.3772, 0.0);
  ScGrappler *sg = new ScGrappler (25.0);
  TrGrappler *tg = new TrGrappler;
  RoGrappler *rg = new RoGrappler (Vect (0.0, 0.0, 1.0));
//  sg -> ScaleZoft () . BecomeLike (sv);
  rg -> AngleZoft () . BecomeLike (sf);
  tg -> TranslationZoft () . BecomeLike (sv);
  s_nodal -> AssuredGrapplerPile () -> AppendGrappler (sg);
  s_nodal -> AssuredGrapplerPile () -> AppendGrappler (rg);
  s_nodal -> AssuredGrapplerPile () -> AppendGrappler (tg);

  SinuVect ss (Vect (0.1), 1.8, Vect (1.0));
  ScGrappler *esgy = new ScGrappler;
  esgy -> ScaleZoft () . BecomeLike (ss);
  dr_no -> Scale (25.0);
  dr_no -> AssuredGrapplerPile () -> AppendGrappler (esgy);
  dr_no -> RotateD (Vect (0.0, 0.0, 1.0), 30.0);
  dr_no -> Translate (36.0, 18.0, 8.0);
//  dr_no -> RotateWithCenterD (Vect (0.0, 0.0, 1.0), 30.0,
//                              Vect (36.0, 18.0, 8.0));

  zone.Run ();

  return 0;
}
