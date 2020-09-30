#include <GraphicsApplication.hpp>

#include <bgfx_utils.hpp>
#include <Renderable.hpp>
#include <VideoSystem.hpp>

#include <stdio.h>

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

namespace charm
{

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

bool GraphicsApplication::InitWindowingAndGraphics ()
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

void GraphicsApplication::ShutDownGraphics ()
{
  bgfx::shutdown();
  glfwTerminate();
}

void GraphicsApplication::Render ()
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
    r->Draw();

  bgfx::frame ();
}

FrameTime *s_app_frame_time{nullptr};

GraphicsApplication::GraphicsApplication ()
  : window {nullptr},
    m_scene_graph_layer {new Layer}
{
}

GraphicsApplication::~GraphicsApplication ()
{
}

bool GraphicsApplication::StartUp ()
{
  if (! s_app_frame_time)
    s_app_frame_time = new FrameTime;


  bool ret = InitWindowingAndGraphics();
  VideoSystem::Initialize ();

  return ret;
}

Node *s_nodal = nullptr;

bool GraphicsApplication::Update ()
{
  GetFrameTime()->UpdateTime();

  glfwPollEvents();

  VideoSystem *vs = VideoSystem::GetSystem();
  assert (vs);
  vs->PollMessages();
  vs->UploadFrames();

  UpdateSceneGraph ();

  Render ();

  return true;
}

void GraphicsApplication::UpdateSceneGraph()
{
  m_scene_graph_layer->GetRootNode()->UpdateTransformsHierarchically();
  m_scene_graph_layer->GetRootNode()->EnumerateRenderables();
}

MoltoSprinkler &GetSprinkler ()
{
}

void GraphicsApplication::ShutDownSceneGraph()
{
  delete m_scene_graph_layer;
  m_scene_graph_layer = nullptr;
}

bool GraphicsApplication::ShutDown ()
{
  ShutDownSceneGraph ();
  VideoSystem::ShutDown();
  ShutDownGraphics ();

  delete s_app_frame_time;
  s_app_frame_time = nullptr;

  return true;
}

FrameTime *GraphicsApplication::GetFrameTime ()
{
  return s_app_frame_time;
}

Layer &GraphicsApplication::GetSceneLayer ()
{
  return *m_scene_graph_layer;
}

}
