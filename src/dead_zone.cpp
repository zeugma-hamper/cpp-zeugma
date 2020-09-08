#include <application.hpp>
#include <base_types.hpp>
#include <utils.hpp>

#include <AnimationSystem.hpp>
#include <FrameTime.hpp>
#include <SceneGraph.hpp>

#include <bgfx_utils.hpp>

#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/file.h>
#include <bx/filepath.h>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stdio.h>

using namespace charm;

class dead_zone final : public charm::application
{
 public:
  dead_zone ();
  ~dead_zone () override final;

  bool start_up ()  override final;
  bool update ()    override final;
  bool shut_down () override final;

  bool init_windowing_and_graphics ();
  void shut_down_graphics ();
  void render ();

  void update_scene_graph ();

  FrameTime &get_frame_time ();
  AnimationSystem &get_animation_system ();

 protected:
  FrameTime m_frame_time;
  AnimationSystem m_animation_system;

  GLFWwindow *window;

  Layer m_scene_graph_layer;
};

class RectangleRenderable final : public Renderable
{
 public:
  RectangleRenderable ()
    : Renderable (),
      program {BGFX_INVALID_HANDLE},
      vbh {BGFX_INVALID_HANDLE}
  {
    struct vvertex
    {
      glm::vec3 position;
      glm::vec4 uv;
    };

    const vvertex combined[] = {
      {{-0.5f,  0.5, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
      {{-0.5f, -0.5, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
      {{ 0.5f,  0.5, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
      {{ 0.5f, -0.5, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
    };

    bgfx::VertexLayout layout;
    layout.begin ()
      .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .add (bgfx::Attrib::Color0,   4, bgfx::AttribType::Float)
      .end ();

    const bgfx::Memory *vb_mem = bgfx::copy (combined, 4 * sizeof (vvertex));
    vbh = bgfx::createVertexBuffer(vb_mem, layout);

    // see note about building shaders above
    bx::FilePath shader_path = "vs_quad.bin";
    bgfx::ShaderHandle vs = create_shader (shader_path);
    shader_path = bx::StringView("fs_quad.bin");
    bgfx::ShaderHandle fs = create_shader (shader_path);

    if (bgfx::isValid(vs) && bgfx::isValid (fs))
      {
        fprintf (stderr, "creating shader\n");
        program = bgfx::createProgram(vs, fs, true);
      }
    else
      {
        bgfx::destroy (vs);
        bgfx::destroy (fs);
      }
  }

  void draw (Node *_node)
  {
    bgfx::setTransform(&_node->get_absolute_model_transformation());
    bgfx::setVertexBuffer(0, vbh, 0, 4);
    bgfx::setState (BGFX_STATE_WRITE_RGB |
                    BGFX_STATE_PT_TRISTRIP |
                    BGFX_STATE_WRITE_Z);
    bgfx::submit(0, program, m_graph_id);
  }

 protected:
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;
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
  application::stop_running();

  fprintf (stderr, "glfw error: %s\n", _msg);
}

//void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
static void glfw_key_callback(GLFWwindow *window, int key, int, int action, int)
{
  if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
      && action == GLFW_RELEASE)
    {
      glfwSetWindowShouldClose (window, GLFW_TRUE);
      application::stop_running();
    }
}

bool dead_zone::init_windowing_and_graphics ()
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

void dead_zone::shut_down_graphics ()
{
  bgfx::shutdown();
  glfwTerminate();
}

void dead_zone::render ()
{
  bgfx::touch (0);

  for (Renderable *r : m_scene_graph_layer.get_renderables())
    r->draw();

  bgfx::frame ();
}

dead_zone::dead_zone ()
  : window {nullptr}
{
}

dead_zone::~dead_zone ()
{
}

bool dead_zone::start_up ()
{
  return init_windowing_and_graphics();
}

bool dead_zone::update ()
{
  m_frame_time.update_time ();

  glfwPollEvents();

  m_animation_system.update_animations(m_frame_time.current_time(),
                                       m_frame_time.current_delta());

  render ();

  return true;
}

void dead_zone::update_scene_graph()
{
  m_scene_graph_layer.root_node()->update_transformations();
  m_scene_graph_layer.root_node()->enumerate_renderables();
}

bool dead_zone::shut_down ()
{
  shut_down_graphics ();

  return true;
}

FrameTime &dead_zone::get_frame_time ()
{
  return m_frame_time;
}

AnimationSystem &dead_zone::get_animation_system ()
{
  return m_animation_system;
}

int main (int, char **)
{

  dead_zone zone;
  zone.run ();

  return 0;
}
