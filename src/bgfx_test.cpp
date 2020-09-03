
#include <utils.hpp>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <stdio.h>

using namespace charm;

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
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

  while (! glfwWindowShouldClose(window) || ! s_should_continue)
    {
      glfwPollEvents();
      bgfx::frame();
    }

  bgfx::shutdown();
  glfwTerminate();

  return 0;
}
