#include <application.hpp>
#include <base_types.hpp>
#include <utils.hpp>

#include <AnimationSystem.hpp>
#include <DecodePipeline.hpp>
#include <FrameTime.hpp>
#include <PipelineTerminus.hpp>
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

#include <glm/gtc/type_ptr.hpp>

#include <string_view>

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
  void shut_down_scene_graph ();
  void render ();

  void update_scene_graph ();

  FrameTime &get_frame_time ();
  AnimationSystem &get_animation_system ();

  Layer &get_scene_layer ();

 protected:
  FrameTime m_frame_time;
  AnimationSystem m_animation_system;

  GLFWwindow *window;

  Layer *m_scene_graph_layer;
};

class VideoRenderable final : public Renderable
{
 public:
  VideoRenderable (std::string_view _uri)
    : Renderable (),
      m_program {BGFX_INVALID_HANDLE},
      m_texture {BGFX_INVALID_HANDLE},
      m_uni_vid_texture {BGFX_INVALID_HANDLE},
      m_video_pipeline {nullptr},
      m_terminus {nullptr}
  {
    m_uni_vid_texture = bgfx::createUniform("u_video_texture", bgfx::UniformType::Sampler);
    m_uni_aspect_ratio = bgfx::createUniform("u_aspect_ratio", bgfx::UniformType::Vec4);

    // see compile_shader.sh in project root
    ProgramResiduals ps = create_program ("video_vs.bin", "video_fs.bin", true);
    m_program = ps.program;

    m_terminus = new BasicPipelineTerminus (false);
    m_video_pipeline = new DecodePipeline;
    m_video_pipeline->open (_uri, m_terminus);
    m_video_pipeline->play();
  }

  ~VideoRenderable () override
  {
    bgfx::destroy (m_uni_aspect_ratio);
    bgfx::destroy (m_uni_vid_texture);
    bgfx::destroy (m_program);
    bgfx::destroy (m_texture);

    delete m_video_pipeline;
    m_terminus = nullptr;
  }

  // static int calculate_alignment (int _stride)
  // {
  //   assert (_stride > 0);

  //   if (! (_stride & 7))
  //     return 8;
  //   else if (! (_stride & 3))
  //     return 4;
  //   else if (! (_stride & 1))
  //     return 2;

  //   return 1;
  // }

  struct video_frame_holder
  {
    gst_ptr<GstSample> sample;
    GstVideoFrame video_frame;

    video_frame_holder (gst_ptr<GstSample> const &_sample, GstVideoInfo *_info)
      : sample {_sample}
    {
      GstBuffer *buffer = gst_sample_get_buffer(_sample.get ());
      gst_video_frame_map (&video_frame, _info, buffer, GST_MAP_READ);
    }

    ~video_frame_holder ()
    {
      gst_video_frame_unmap(&video_frame);
    }
  };

  static void free_video_holder (void *, void *_user_data)
  {
    video_frame_holder *holder = (video_frame_holder *)_user_data;
    delete holder;
  }

  void upload_sample (gst_ptr<GstSample> const &_sample)
  {
    GstCaps *sample_caps = gst_sample_get_caps(_sample.get ());
    GstVideoInfo video_info;
    int width, height, components, stride;

    if (! sample_caps)
      return;

    gst_video_info_init(&video_info);
    if (! gst_video_info_from_caps (&video_info, sample_caps))
      return;

    if (! GST_VIDEO_INFO_IS_RGB (&video_info))
      return;

    width = GST_VIDEO_INFO_WIDTH (&video_info);
    height = GST_VIDEO_INFO_HEIGHT (&video_info);
    components = GST_VIDEO_INFO_N_COMPONENTS (&video_info);
    //RGB is all one plane
    stride = GST_VIDEO_INFO_PLANE_STRIDE(&video_info, 0);
    // align = calculate_alignment (stride);
    (void)components;

    if (! bgfx::isValid(m_texture))
      {
        fprintf (stderr, "create video texture\n");
        bgfx::TextureFormat::Enum const formats[5]
          { bgfx::TextureFormat::Unknown, //shouldn't happen
            bgfx::TextureFormat::R8,
            bgfx::TextureFormat::RG8,
            bgfx::TextureFormat::RGB8,
            bgfx::TextureFormat::RGBA8 };

        m_texture = bgfx::createTexture2D(width, height, false, 1,
                                          formats[components],
                                          BGFX_SAMPLER_UVW_CLAMP |
                                          BGFX_SAMPLER_POINT);
      }

    video_frame_holder *frame_holder = new video_frame_holder (_sample, &video_info);

    const bgfx::Memory *mem = bgfx::makeRef (GST_VIDEO_FRAME_PLANE_DATA(&frame_holder->video_frame, 0),
                                             GST_VIDEO_FRAME_SIZE(&frame_holder->video_frame),
                                             free_video_holder, frame_holder);

    bgfx::updateTexture2D(m_texture, 0, 0, 0, 0, width, height, mem, stride);
  }

  void update () override
  {
    // static int count = 0;
    // if (++count % 10 == 0)
    //   fprintf (stderr, "v.r. update\n");

    if (m_video_pipeline)
      {

        m_video_pipeline->poll_messages ();
        gst_ptr<GstSample> new_sample = m_terminus->fetch_clear_sample();

        if (new_sample)
          upload_sample(new_sample);
      }
  }

  void draw () override
  {
    if (! bgfx::isValid(m_texture))
      return;

    //later: BGFX_STATE_WRITE_A |
    //       BGFX_ uhhh... blending
    u64 const state = BGFX_STATE_WRITE_RGB |
      BGFX_STATE_PT_TRISTRIP |
      BGFX_STATE_WRITE_Z;

    bgfx::setTransform(&m_node->get_absolute_model_transformation());
    bgfx::setState(state);
    bgfx::setVertexCount(4);
    bgfx::setTexture(0, m_uni_vid_texture, m_texture);
    glm::vec4 unity {1.0f};
    bgfx::setUniform(m_uni_aspect_ratio, glm::value_ptr (unity));
    bgfx::submit(0, m_program);
  }

 private:
  bgfx::ProgramHandle m_program;
  bgfx::TextureHandle m_texture;
  bgfx::UniformHandle m_uni_vid_texture;
  bgfx::UniformHandle m_uni_aspect_ratio;
  DecodePipeline *m_video_pipeline;
  BasicPipelineTerminus *m_terminus;
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
      glm::vec4 color;
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
    bx::FilePath shader_path = "quad_vs.bin";
    bgfx::ShaderHandle vs = create_shader (shader_path);
    shader_path = bx::StringView("quad_fs.bin");
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

  ~RectangleRenderable () override
  {
    bgfx::destroy(vbh);
    bgfx::destroy(program);
  }

  void draw () override
  {
    fprintf (stderr, "rr draw\n");

    bgfx::setTransform(&m_node->get_absolute_model_transformation());
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

  glm::mat4 ident{1.0f};
  bgfx::setViewTransform(0, glm::value_ptr (ident), glm::value_ptr (ident));

  return true;
}

void dead_zone::shut_down_graphics ()
{
  bgfx::shutdown();
  glfwTerminate();
}

void dead_zone::render ()
{
  for (Renderable *r : m_scene_graph_layer->get_renderables())
    r->update ();

  bgfx::touch (0);

  glm::mat4 ident{1.0f};
  bgfx::setViewTransform(0, glm::value_ptr (ident), glm::value_ptr (ident));

  for (Renderable *r : m_scene_graph_layer->get_renderables())
    r->draw();

  bgfx::frame ();
}

dead_zone::dead_zone ()
  : window {nullptr},
    m_scene_graph_layer {new Layer}
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

  update_scene_graph ();

  render ();

  return true;
}

void dead_zone::update_scene_graph()
{
  m_scene_graph_layer->root_node()->update_transformations();
  m_scene_graph_layer->root_node()->enumerate_renderables();
}

void dead_zone::shut_down_scene_graph()
{
  delete m_scene_graph_layer;
  m_scene_graph_layer = nullptr;
}

bool dead_zone::shut_down ()
{
  shut_down_scene_graph ();
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

Layer &dead_zone::get_scene_layer ()
{
  return *m_scene_graph_layer;
}

int main (int, char **)
{
  dead_zone zone;
  if (! zone.start_up ())
    return -1;

  Layer &layer = zone.get_scene_layer();

  Node *node = new Node ();
  VideoRenderable *renderable
    = new VideoRenderable ("file:///home/blake/tlp/tamper-blu-mkv/The Fall_t00.mkv");

  node->append_renderable(renderable);
  layer.root_node()->append_child(node);

  zone.run ();

  return 0;
}
