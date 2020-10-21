#include <GraphicsApplication.hpp>

#include <bgfx_utils.hpp>
#include <GLFWWaterWorks.hpp>
#include <Renderable.hpp>
#include <VideoSystem.hpp>

#include <conjure-from-toml.h>
#include <toml.hpp>

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

// TOML-related key names
static const char *s_window_toml_file_windows = "windows";
static const char *s_window_toml_file_trefoils = "trefoils";


static const char *s_window_toml_name = "name";
static const char *s_window_toml_pos = "pos";
static const char *s_window_toml_size = "size";
static const char *s_window_toml_rgba = "rgba";
static const char *s_window_toml_ds = "ds";
static const char *s_window_toml_decorated = "decorated";

static const char *s_trefoil_toml_name = "name";
static const char *s_trefoil_toml_maes = "maes";
static const char *s_trefoil_toml_offset = "offset";
static const char *s_trefoil_toml_size = "size";

struct WindowTOML
{
  std::string name;
  std::array<i32, 2> pos = {0, 0};
  std::array<u32, 2> size = {1920, 1080};
  std::array<u8, 4> rgba = {8, 8, 8, 8};
  std::array<u8, 2> ds = {24, 8};
  bool decorated = false;

  void from_toml (toml::value const &_v)
  {
    name = toml::find<std::string> (_v, s_window_toml_name);
    pos = toml::find<std::array<i32, 2>> (_v, s_window_toml_pos);
    size = toml::find<std::array<u32, 2>> (_v, s_window_toml_size);
    rgba = toml::find<std::array<u8, 4>> (_v, s_window_toml_rgba);
    ds = toml::find<std::array<u8, 2>> (_v, s_window_toml_ds);
    decorated = toml::find<bool> (_v, s_window_toml_decorated);
  }
};

struct TrefoilTOML
{
  std::string name;
  std::string maes_name;
  std::array<u32, 2> offset {0, 0};
  std::array<u32, 2> size {u32 (-1), u32 (-1)};

  void from_toml (toml::value const &_v)
  {
    name = toml::find<std::string> (_v, s_trefoil_toml_name);
    maes_name = toml::find<std::string> (_v, s_trefoil_toml_maes);
    offset = toml::find<std::array<u32, 2>> (_v, s_trefoil_toml_offset);
    size = toml::find<std::array<u32, 2>> (_v, s_trefoil_toml_size);
  }
};

static bool ReadWindowTOML (std::string_view _file, WindowTOML &_window,
                            std::vector<TrefoilTOML> &_views)
{
  try
    {
      auto config = toml::parse (_file);
      std::vector<WindowTOML> wins
        = toml::find<std::vector<WindowTOML>> (config, s_window_toml_file_windows);
      _window = wins[0];
      _views = toml::find<std::vector<TrefoilTOML>> (config, s_window_toml_file_trefoils);
    }
  catch (std::runtime_error &_error)
    {
      fprintf (stderr, "error loading %s:\n%s\n",
               _file.data(), _error.what ());

      return false;
    }
  catch (toml::exception &_error)
    {
      fprintf (stderr, "toml error %s:%s\n%s\n",
               _error.location().file_name().c_str(),
               _error.location().line_str().c_str(),
               _error.what());
      return false;
    }

  return true;
}

bool GraphicsApplication::InitWindowingAndGraphics ()
{
  WindowTOML win;
  std::vector<TrefoilTOML> trefoils;
  if (! ReadWindowTOML ("../configs/window-config.toml", win, trefoils))
    return false;

  glfwSetErrorCallback(glfw_error_callback);
  if (! glfwInit())
    ERROR_RETURN_VAL ("error initializing glfw", false);

  glfwWindowHint (GLFW_RED_BITS,     win.rgba[0]);
  glfwWindowHint (GLFW_GREEN_BITS,   win.rgba[1]);
  glfwWindowHint (GLFW_BLUE_BITS,    win.rgba[2]);
  glfwWindowHint (GLFW_ALPHA_BITS,   win.rgba[3]);
  glfwWindowHint (GLFW_DEPTH_BITS,   win.ds[0]);
  glfwWindowHint (GLFW_STENCIL_BITS, win.ds[1]);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow (win.size[0], win.size[1],
                                         win.name.c_str (), nullptr, nullptr);
  if (! window)
    ERROR_RETURN_VAL ("couldn't create window", false);

  // install window key and mouse handler
  GLFWWaterWorks *glfw_ww = new GLFWWaterWorks (window);
  AppendWaterWorks(glfw_ww);
  glfw_ww->Drain (&m_event_sprinkler); //call poll events, basically

  glfwSetWindowPos(window, win.pos[0], win.pos[1]);
  glfwSetWindowSize(window, win.size[0], win.size[1]);
  int glfw_width, glfw_height;
  glfwGetWindowSize(window, &glfw_width, &glfw_height);

  bgfx::renderFrame();
  bgfx::Init init;

  init.type = bgfx::RendererType::OpenGL;
  init.platformData.ndt = glfwGetX11Display();
  init.platformData.nwh = (void *)glfwGetX11Window(window);
  init.platformData.context = glfwGetGLXContext(window);
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


  std::string const maes_path = "../config/maes-config.toml";
  i32 const num_maes = NumMaesesFromTOML(maes_path);

  for (i32 i = 0; i < num_maes; ++i)
    if (PlatonicMaes *mm = MaesFromTOML(maes_path, i); mm)
      m_maes.push_back (mm);

  f64 width_scale = glfw_width / f64 (win.size[0]);
  f64 height_scale = glfw_height / f64 (win.size[1]);
  i32 const tsize = i32 (trefoils.size ());
  for (i32 i = 0; i < tsize; ++i)
    {
      TrefoilTOML const &tt = trefoils[i];
      PlatonicMaes *maes = FindMaesByName(tt.maes_name);
      if (! maes)
        {
          fprintf (stderr, "Trefoil: couldn't find Maes by name '%s'\n", tt.maes_name.c_str());
          continue;
        }
      Bolex *bolex = CameraFromMaes(*maes);
      CMVTrefoil *tr = new CMVTrefoil (bolex, maes);
      tr->SetName (tt.name);
      tr->view.view_id = i;
      tr->view.fb_pix_l = (i64)std::round (width_scale  * tt.offset[0]);
      tr->view.fb_pix_t = (i64)std::round (height_scale * tt.offset[1]);
      tr->view.fb_pix_r = (i64)std::round (width_scale  * (tt.offset[0] + tt.size[0]));
      tr->view.fb_pix_b = (i64)std::round (height_scale * (tt.offset[1] + tt.size[1]));

      m_trefoils.push_back (tr);
    }

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
    r->Draw(0);

  bgfx::frame ();
}

FrameTime *s_app_frame_time {nullptr};
GraphicsApplication *s_graphics_application_instance {nullptr};

GraphicsApplication::GraphicsApplication ()
  : m_window {nullptr},
    m_scene_graph_layer {new Layer}
{
  s_graphics_application_instance = this;
}

GraphicsApplication::~GraphicsApplication ()
{
  s_graphics_application_instance = nullptr;
}

bool GraphicsApplication::StartUp ()
{
  if (! s_app_frame_time)
    s_app_frame_time = new FrameTime;


  bool ret = InitWindowingAndGraphics();
  VideoSystem::Initialize ();

  return ret;
}

bool GraphicsApplication::RunOneCycle ()
{
  GetFrameTime()->UpdateTime();

  for (ZePublicWaterWorks *ww : m_event_drainage)
    ww->Drain (&m_event_sprinkler);

  VideoSystem *vs = VideoSystem::GetSystem();
  assert (vs);
  vs->PollMessages();
  vs->UploadFrames();

  UpdateSceneGraph (-1, 0.0);

  Render ();

  return true;
}

void GraphicsApplication::UpdateSceneGraph(i64 ratch, f64 thyme)
{
  m_scene_graph_layer->GetRootNode()
    -> UpdateTransformsHierarchically (ratch, thyme);
  m_scene_graph_layer->GetRootNode()->EnumerateGraph();
}

MultiSprinkler &GraphicsApplication::GetSprinkler ()
{
  return m_event_sprinkler;
}

void GraphicsApplication::AppendWaterWorks (ZePublicWaterWorks *_pub)
{
  if (_pub)
    m_event_drainage.push_back(_pub);
}

void GraphicsApplication::RemoveWaterWorks (ZePublicWaterWorks *_pub)
{
  if (! _pub)
    return;

  m_event_drainage.erase (std::find (m_event_drainage.begin (), m_event_drainage.end (), _pub));
  delete _pub;
}

ZePublicWaterWorks *GraphicsApplication::ExciseWaterWorks (ZePublicWaterWorks *_pub)
{
  auto const it = std::find (m_event_drainage.begin (), m_event_drainage.end (), _pub);
  ZePublicWaterWorks *ww = it != m_event_drainage.end () ? *it : nullptr;
  m_event_drainage.erase (it);

  return ww;
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

i32 GraphicsApplication::NumMaeses ()  const
{
  return i32 (m_maes.size ());
}

PlatonicMaes *GraphicsApplication::NthMaes (i32 ind) const
{
  if (ind < NumMaeses())
    return m_maes[ind];

  return nullptr;
}

PlatonicMaes *GraphicsApplication::FindMaesByName (std::string_view _name) const
{
  for (PlatonicMaes *pm : m_maes)
    if (pm->Name() == _name)
      return pm;

  return nullptr;
}

i32 GraphicsApplication::NumRenderLeaves ()  const
{
  return i32 (m_trefoils.size ());
}

CMVTrefoil *GraphicsApplication::NthRenderLeaf (i32 ind) const
{
  if (ind < NumRenderLeaves())
    return m_trefoils[ind];

  return nullptr;
}

CMVTrefoil *GraphicsApplication::FindRenderLeafByName (std::string_view _name) const
{
  for (CMVTrefoil *cmv : m_trefoils)
    if (cmv->Name() == _name)
      return cmv;

  return nullptr;
}

}
