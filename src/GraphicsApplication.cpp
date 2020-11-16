
#include <GraphicsApplication.hpp>

#include <bgfx_utils.hpp>
#include <Bolex.h>
#include <GLFWWaterWorks.hpp>
#include <OSCWandWaterWorks.hpp>
#include <Renderable.hpp>
#include <VideoSystem.hpp>

#include <conjure-from-toml.h>
#include <vector_interop.hpp>

#include <stdio.h>


#define ERROR_RETURN_VAL(MSG, VAL)                 \
  { fprintf (stderr, "%s\n", MSG);                 \
    return VAL;                                    \
  }

#define ERROR_RETURN(MSG)                          \
  { fprintf (stderr, "%s\n", MSG);                 \
    return;                                        \
  }


namespace charm
{


static void glfw_error_callback (int, const char *_msg)
{
  Application::StopRunning();

  fprintf (stderr, "glfw error: %s\n", _msg);
}

bool GraphicsApplication::InitWindowingAndGraphics ()
{
  WindowTOML win;
  std::vector <TrefoilTOML> trefoils;
  if (! ReadWindowTOMLFile ("../configs/window-config.toml", win, trefoils))
    return false;

  std::array<u32, 2> max_dimensions {0u, 0u};
  for (auto const &t : trefoils)
    {
      max_dimensions[0] = std::max (max_dimensions[0], t.offset[0] + t.size[0]);
      max_dimensions[1] = std::max (max_dimensions[1], t.offset[1] + t.size[1]);
    }

  glfwSetErrorCallback(glfw_error_callback);
  if (! glfwInit ())
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
  glfwWindowHint (GLFW_DECORATED, win.decorated ? GLFW_TRUE : GLFW_FALSE);

  GLFWwindow *window = glfwCreateWindow (win.size[0], win.size[1],
                                         win.name.c_str (), nullptr, nullptr);
  if (! window)
    ERROR_RETURN_VAL ("couldn't create window", false);

  // install window key and mouse handler
  GLFWWaterWorks *glfw_ww = new GLFWWaterWorks (window);
  AppendWaterWorks (glfw_ww);
  //call poll events, basically
  glfw_ww->Drain (&m_event_sprinkler);

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

  // bgfx::setViewRect(0, 0, 0, glfw_width, glfw_height);
  // bgfx::setViewScissor(0);
  // bgfx::setViewClear (0, BGFX_CLEAR_COLOR, 0x808080FF);
  // // use depth integer to order GPU draw submission
  // // drawables are enumerated by order in scene graph,
  // // so that id number is used as the depth.
  // bgfx::setViewMode (0, bgfx::ViewMode::DepthAscending);

  // glm::mat4 view_transform = glm::lookAt (glm::vec3 {0.0f, 0.0f, 10.0f},
  //                                         glm::vec3 {0.0f, 0.0f, 2.0f},
  //                                         glm::vec3 {0.0f, 1.0f, 0.0f});
  // glm::mat4 proj_transform = glm::perspectiveFov (glm::pi<float> ()/4.0f,
  //                                                 1920.0f, 1080.0f, 0.5f, 10.0f);
  // bgfx::setViewTransform(0, glm::value_ptr (view_transform),
  //                        glm::value_ptr (proj_transform));


  std::string const maes_path = "../configs/maes-config.toml";
  i32 const num_maes = NumMaesesFromTOML(maes_path);
  fprintf (stderr, "num maes is %d\n", num_maes);


  for (i32 i = 0; i < num_maes; ++i)
    if (PlatonicMaes *mm = MaesFromTOML(maes_path, i); mm)
      m_maes.push_back (mm);

  f64 width_scale = glfw_width / f64 (max_dimensions[0]);
  f64 height_scale = glfw_height / f64 (max_dimensions[1]);

  fprintf (stdout, "scaling views by (%.3f, %.3f)\n", width_scale, height_scale);
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
      tr->view.fb_pix_t = (i64)std::round (height_scale * (tt.offset[1] + tt.size[1]));
      tr->view.fb_pix_r = (i64)std::round (width_scale  * (tt.offset[0] + tt.size[0]));
      tr->view.fb_pix_b = (i64)std::round (height_scale * tt.offset[1]);

      bgfx::setViewRect (i,
                         tr->view.fb_pix_l,
                         tr->view.fb_pix_b,
                         tr->view.GetWidth(),
                         tr->view.GetHeight());
      bgfx::setViewScissor (i,
                            tr->view.fb_pix_l,
                            tr->view.fb_pix_b,
                            tr->view.GetWidth(),
                            tr->view.GetHeight());

      //TODO: add color back
      //u8 gray = u8 (global_param_background_gray * 255.0);
      u8 gray = 26u;
      u32 bg_rgba = (0xFF | gray << 8 | gray << 16 | gray << 24);
        bgfx::setViewClear (i,
                          BGFX_CLEAR_COLOR |
                          BGFX_CLEAR_DEPTH |
                          BGFX_CLEAR_STENCIL,
                          bg_rgba);
      // use depth integer to order GPU draw submission
      // drawables are enumerated by order in scene graph,
      // so that id number is used as the depth.
      bgfx::setViewMode (i, bgfx::ViewMode::DepthAscending);

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
  for (Layer *l : m_scene_graph_layers)
    for (Renderable *r : l->GetRenderables())
      r -> Update ();

  for (CMVTrefoil *leaf : m_trefoils)
    {
      u16 vuid = leaf->view.ViewID ();
      bgfx::touch (vuid);

      Bolex *c = leaf->cam;
      glm::mat4 view_transform = glm::lookAt (as_glm (c -> ViewLoc ()),
                                              as_glm (c -> ViewCOI ()),
                                              as_glm (c -> ViewUp ()));
      glm::mat4 proj_transform
        = glm::perspective ((float) c -> ViewVertAngle (),
                            (float) (tan (0.5 * c -> ViewHorizAngle ())
                                     / tan (0.5 * c -> ViewVertAngle ())),
                            (float) c -> NearClipDist (),
                            (float) c -> FarClipDist ());

      bgfx::setViewTransform (vuid, glm::value_ptr (view_transform),
                              glm::value_ptr (proj_transform));

      for (Layer *l : leaf->layers)
        for (Renderable *r  :  l->GetRenderables())
          r -> Draw (vuid);
    }

  bgfx::frame ();
}

FrameTime *s_app_frame_time {nullptr};
GraphicsApplication *s_graphics_application_instance {nullptr};

GraphicsApplication::GraphicsApplication ()
  : m_global_ratchet (0),
    m_window {nullptr},
    m_scene_graph_layers {new Layer}
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

  AppendWaterWorks (new OSCWandWaterWorks);

  return ret;
}

bool GraphicsApplication::RunOneCycle ()
{
  GetFrameTime()->UpdateTime();
  f64 global_frame_thyme = GetFrameTime () -> GetCurrentTime ();
  m_global_ratchet += 8;

  for (ZePublicWaterWorks *ww : m_event_drainage)
    ww->Drain (&m_event_sprinkler);

  DoWhatThouWilt (m_global_ratchet, global_frame_thyme);

  if (ProtoZoftThingGuts::IsMassBreathing ())
    ProtoZoftThingGuts::MassBreather ()
      -> Inhale (m_global_ratchet, global_frame_thyme);

  VideoSystem *vs = VideoSystem::GetSystem();
  assert (vs);
  vs->PollMessages();
  vs->UploadFrames();

  UpdateSceneGraph (m_global_ratchet, global_frame_thyme);
  UpdateRenderLeaves (m_global_ratchet, global_frame_thyme);

  for (CMVTrefoil *leaf  :  m_trefoils)
    leaf -> Inhale (m_global_ratchet, global_frame_thyme);

  Render ();

  return true;
}

void GraphicsApplication::UpdateSceneGraph(i64 ratch, f64 thyme)
{
  std::array<graph_id, 2> ids {0, 0};
  for (Layer *layer : m_scene_graph_layers)
    {
      layer->GetRootNode()
        ->UpdateTransformsHierarchically (ratch, thyme);
      ids = layer->GetRootNode()->EnumerateGraph(ids[0], ids[1]);
      layer->SortFrontiers ();
    }
}

void GraphicsApplication::UpdateRenderLeaves (i64 ratch, f64 thyme)
{
  for (CMVTrefoil *leaf  :  m_trefoils)
    leaf -> Inhale (ratch, thyme);
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

  m_event_drainage.erase (std::find (m_event_drainage.begin (), m_event_drainage.end (), _pub),
                          m_event_drainage.end ());
  delete _pub;
}

ZePublicWaterWorks *GraphicsApplication::ExciseWaterWorks (ZePublicWaterWorks *_pub)
{
  auto const it = std::find (m_event_drainage.begin (), m_event_drainage.end (), _pub);

  if (it == m_event_drainage.end ())
    return nullptr;

  ZePublicWaterWorks *ww = *it;
  m_event_drainage.erase (it);

  return ww;
}

void GraphicsApplication::ShutDownSceneGraph()
{
  for (Layer *layer : m_scene_graph_layers)
    delete layer;
  m_scene_graph_layers.clear();
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

GraphicsApplication *GraphicsApplication::GetApplication ()
{
  return s_graphics_application_instance;
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

PlatonicMaes *GraphicsApplication::ClosestIntersectedMaes (const Vect &frm,
                                                           const Vect &aim,
                                                           Vect *hit_point)
{ PlatonicMaes *close_m = NULL;
  Vect close_p, hit;
  f64 close_d;

  i32 cnt = NumMaeses ();
  for (i32 q = 0  ;  q < cnt  ;  ++q)
    { PlatonicMaes *emm = NthMaes (q);
      if (G::RayRectIntersection (frm, aim,
                                  emm -> Loc (), emm -> Over (),
                                  emm -> Up (), emm -> Width (),
                                  emm -> Height (), &hit))
        { f64 d = hit . DistFrom (frm);
          if (! close_m  ||  d < close_d)
            { close_m = emm;
              close_p = hit;
              close_d = d;
            }
        }
    }
  if (hit_point)
    *hit_point = close_p;
  return close_m;
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

Layer *GraphicsApplication::GetSceneLayer ()
{
  return m_scene_graph_layers[0];
}

Layer *GraphicsApplication::GetNthSceneLayer (i32 nth)
{
  assert (nth < i64 (m_scene_graph_layers.size ()));
  return m_scene_graph_layers[nth];
}

void GraphicsApplication::AppendSceneLayer (Layer *layer)
{
  if (layer == nullptr)
    return;

  m_scene_graph_layers.push_back (layer);
}

i32 GraphicsApplication::NumSceneLayers () const
{
  return i32 (m_scene_graph_layers.size ());
}

}
