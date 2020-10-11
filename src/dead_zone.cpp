#include <application.hpp>
#include <base_types.hpp>
#include <class_utils.hpp>

#include <DecodePipeline.hpp>
#include <FrameTime.hpp>
#include <Layer.hpp>
#include <Node.hpp>
#include <PipelineTerminus.hpp>
#include <Renderable.hpp>
#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>
#include <VideoSystem.hpp>

#include <BlockTimer.hpp>

#include <bgfx_utils.hpp>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <charm_glm.hpp>

#include <string_view>

#include <stdio.h>

#include <Matte.hpp>

#include <type_int.hpp>

using namespace charm;

class dead_zone final : public charm::Application
{
 public:
  dead_zone ();
  ~dead_zone () override final;

  bool StartUp     () override final;
  bool RunOneCycle () override final;
  bool ShutDown    () override final;

  bool InitWindowingAndGraphics ();
  void ShutDownGraphics ();
  void ShutDownSceneGraph ();
  void Render ();

  void UpdateSceneGraph (i64 ratch, f64 thyme);

  static FrameTime *GetFrameTime ();

  Layer &GetSceneLayer ();

 protected:

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

  glm::mat4 view_transform = glm::lookAt (glm::vec3 {0.0f, 0.0f, 10.0f},
                                          glm::vec3 {0.0f, 0.0f, 2.0f},
                                          glm::vec3 {0.0f, 1.0f, 0.0f});
  glm::mat4 proj_transform = glm::perspectiveFov (glm::pi<float> ()/4.0f,
                                                  1920.0f, 1080.0f, 0.5f, 10.0f);
  bgfx::setViewTransform(0, glm::value_ptr (view_transform),
                         glm::value_ptr (proj_transform));

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

  glm::mat4 view_transform = glm::lookAt (glm::vec3 {0.0f, 0.0f, 10.0f},
                                          glm::vec3 {0.0f, 0.0f, 2.0f},
                                          glm::vec3 {0.0f, 1.0f, 0.0f});
  glm::mat4 proj_transform = glm::perspective (47.0f, 1920.0f/1080.0f, 0.5f, 10.0f);
  bgfx::setViewTransform(0, glm::value_ptr (view_transform),
                         glm::value_ptr (proj_transform));

  for (Renderable *r : m_scene_graph_layer->GetRenderables())
    r->Draw(0);

  bgfx::frame ();
}

FrameTime *s_dead_zone_frame_time{nullptr};

dead_zone::dead_zone ()
  : window {nullptr},
    m_scene_graph_layer {new Layer}
{
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

bool dead_zone::RunOneCycle ()
{
  GetFrameTime()->UpdateTime();

  glfwPollEvents();

  VideoSystem *video_system = VideoSystem::GetSystem ();
  video_system->PollMessages();
  video_system->UploadFrames();

  UpdateSceneGraph (-1, 0.0);

  {
    //BlockTimer ("render");
    Render ();
  }

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


  std::vector<FilmInfo> configs = ReadFilmInfo ("../configs/film-config.toml");
  assert (configs.size () > 0);

  std::vector<FilmGeometry> geom = ReadFileGeometry("../configs/mattes.toml");
  assert (geom.size () > 0);
  MergeFilmInfoGeometry(configs, geom);

  //FilmInfo &film_info = configs[0];
  FilmInfo &film_info = configs.back ();
  assert (film_info.clips.size () > 0);
  ClipInfo &clip_info = film_info.clips[0];

  // std::string file
  //   = "file:///home/blake/tlp/tamper-blu-mkv/the-fall-blu.mov";

  std::string uri = std::string ("file://") + film_info.film_path.string ();
  MattedVideoRenderable *renderable
    = new MattedVideoRenderable (film_info, clip_info);

  printf ("clip dims: [%u, %u] - [%u, %u]\n",
          clip_info.geometry.dir_geometry.min[0],
          clip_info.geometry.dir_geometry.min[1],
          clip_info.geometry.dir_geometry.max[0],
          clip_info.geometry.dir_geometry.max[1]);
  //VideoRenderable *renderable = new VideoRenderable (uri);

  s_nodal->AppendRenderable(renderable);
  layer.GetRootNode()->AppendChild(s_nodal);

  zone.Run ();

  return 0;
}
