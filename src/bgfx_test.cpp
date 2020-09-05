
#include <utils.hpp>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/file.h>
#include <bx/filepath.h>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>

#include <vector>

#include <stdio.h>

#include <base_types.hpp>

using namespace charm;


// BUILDING SHADERS
// i haven't automated building shaders yet, so here is the command i
// used from the root of the project directory:
// bld/shaderc -i bgfx/src -f src/vs_quad.sc -o bld/vs_quad.bin --type vertex --varyingdef src/quad_varying.def.sc
// bld/shaderc -i bgfx/src -f src/fs_quad.sc -o bld/fs_quad.bin --type fragment --varyingdef src/quad_varying.def.sc
// -i: include directory
// -f: shader file
// --varyingdef: file defining inputs and outputs from shaders

//borrowing from example:
//https://github.com/jpcy/bgfx-minimal-example/blob/master/helloworld.cpp

#define ERROR_RETURN_VAL(MSG, VAL) \
  {                                                \
    fprintf (stderr, "%s\n", MSG);                 \
    return VAL;                                    \
  }

#define ERROR_RETURN(MSG) ERROR_RETURN_VAL(MSG, 1)

bool s_should_continue = true;

void glfw_error_callback (int, const char *_msg)
{
  s_should_continue = false;

  fprintf (stderr, "glfw error: %s\n", _msg);
}

//void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
void glfw_key_callback(GLFWwindow *window, int key, int, int action, int)
{
  if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
      && action == GLFW_RELEASE)
    glfwSetWindowShouldClose (window, GLFW_TRUE);
}

const bgfx::Memory *read_whole_file (bx::FilePath const &_path)
{
  bx::Error error;
  bx::FileReader reader;
  if (! reader.open (_path, &error))
    return nullptr;

  i64 size = reader.seek (0, bx::Whence::End);
  const bgfx::Memory *mem = bgfx::alloc (size);
  reader.seek (0, bx::Whence::Begin);

  if (0 >= reader.read (mem->data, mem->size, &error))
    {
      reader.close ();
      return nullptr;
    }

  reader.close ();
  return mem;
}

bgfx::ShaderHandle create_shader (bx::FilePath const &_path)
{
  const bgfx::Memory *memory = read_whole_file (_path);
  if (! memory)
    return BGFX_INVALID_HANDLE;

  return bgfx::createShader (memory);
}

std::vector<bgfx::UniformHandle> get_shader_uniforms (bgfx::ShaderHandle _sh)
{
  u16 uni_count = bgfx::getShaderUniforms(_sh);
  std::vector<bgfx::UniformHandle> handles{uni_count};
  bgfx::getShaderUniforms (_sh, handles.data (), uni_count);

  return handles;
}


struct Rectangle
{
  bgfx::ProgramHandle program;
  bgfx::VertexBufferHandle vbh;

  Rectangle ()
  {
    program = BGFX_INVALID_HANDLE;
    vbh = BGFX_INVALID_HANDLE;

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

  void draw ()
  {
    bgfx::setVertexBuffer(0, vbh, 0, 4);
    bgfx::setState (BGFX_STATE_WRITE_RGB |
                    BGFX_STATE_PT_TRISTRIP |
                    BGFX_STATE_WRITE_Z);
    bgfx::submit(0, program);
  }

  void clean_up ()
  {
    bgfx::destroy(program);
    bgfx::destroy(vbh);
  }
};

int main (int, char **)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (! glfwInit())
    ERROR_RETURN ("error initializing glfw");

  glfwWindowHint (GLFW_RED_BITS, 8);
  glfwWindowHint (GLFW_GREEN_BITS, 8);
  glfwWindowHint (GLFW_BLUE_BITS, 8);
  glfwWindowHint (GLFW_DEPTH_BITS, 24);
  glfwWindowHint (GLFW_STENCIL_BITS, 8);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);

  GLFWwindow *window = glfwCreateWindow (1920, 1080, "bgfx test", nullptr, nullptr);
  if (! window)
    ERROR_RETURN ("couldn't create window");

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
    ERROR_RETURN("couldn't initialize bgfx");

  Rectangle rectangle;

  bgfx::setViewRect(0, 0, 0, glfw_width, glfw_height);
  fprintf (stderr, "wid x hei: %d x %d\n", glfw_width, glfw_height);
  bgfx::setViewScissor(0);
  bgfx::setViewClear (0, BGFX_CLEAR_COLOR, 0x808080FF);
  while (! glfwWindowShouldClose(window) || ! s_should_continue)
    {
      glfwPollEvents();
      bgfx::touch (0);

      rectangle.draw ();

      bgfx::frame();
    }

  rectangle.clean_up();
  bgfx::shutdown();
  glfwTerminate();

  return 0;
}
