//application
#include <application.hpp>
#include <FrameTime.hpp>
#include <Layer.hpp>

//video, mattes
#include <VideoSystem.hpp>
#include <Matte.hpp>

//Renderables
#include <Renderable.hpp>
#include <RectangleRenderable.hpp>
#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

//base Ze
#include <Zeubject.h>
#include <base_types.hpp>

//camera, surface
#include <Bolex.h>
#include <PlatonicMaes.h>
#include <conjure-from-toml.h>

//animation
#include <ZoftThing.h>
#include <InterpZoft.h>

//events
#include <ZESpatialEvent.h>
#include <RawEventParsing.h>
#include <GeomFumble.h>

//etc.
#include <BlockTimer.hpp>

//dependencies
#include <bgfx_utils.hpp>

#include <charm_glm.hpp>
#include <vector_interop.hpp>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <lo/lo_cpp.h>

//C++, C
#include <unordered_set>
#include <optional>

#include <stdio.h>


//for the demo
#include <TriBand.hpp>
#include <Collage.hpp>

using namespace charm;

class CMVTrefoil  :  public Zeubject
{ public:
  Bolex *cam;
  PlatonicMaes *maes;
  std::vector<Layer *> layers;
  struct BGFXView {
    i32 view_id;  // bgfx's is u16, so we can rep't stuff in neg nos.
    f64 botlef_x, botlef_y;
    f64 wid_frac, hei_frac;  // these four norm'd [0,1] ref'ing window res
    i64 fb_pix_l, fb_pix_b;
    i64 fb_pix_r, fb_pix_t;
    i64 wind_wid, wind_hei;
    u16 ViewID ()  const  { return (u16)view_id; }
  } *b_view;
  CMVTrefoil ()  :  Zeubject (), cam (NULL), maes (NULL), b_view (NULL)
    { }
  i64 Inhale (i64 ratch, f64 thyme)  override
    { if (cam)  cam -> Inhale (ratch, thyme);
      if (maes)  maes -> Inhale (ratch, thyme);
      return 0;
    }
};


Bolex *CameraFromMaes (const PlatonicMaes &m)
{ Bolex *cam = new Bolex;

  Vect nrm = m . Over () . Cross (m . Up ()) . Norm ();
  f64 dst = 0.8 * m . Width ();

  cam -> SetViewDist (dst);
  cam -> SetViewLoc (m . Loc ()  +  dst * nrm);
  cam -> SetViewCOI (m . Loc ());
  cam -> SetViewUp (m . Up ());

  cam -> SetProjectionType (Bolex::ProjType::PERSPECTIVE);

  cam -> SetViewHorizAngle (2.0 * atan (0.5 * m . Width () / dst));
  cam -> SetViewVertAngle (2.0 * atan (0.5 * m . Height () / dst));

  cam -> SetNearAndFarClipDist (0.1, 10.0 * dst);
  return cam;
}

class WandCatcher  :  public Zeubject, public ZESpatialPhagy
{
 public:
  i32 trig_butt_simulcount;
  u64 trig_butt_ident;
  std::unordered_set <std::string> trig_partic;
  std::unordered_set <std::string> elev_partic;
  bool calibrating;
  bool elevating;
  Vect elev_prevpos;
  std::map <std::string, Vect> recentest_pos;
  ZESpatialPhagy *cally;

  WandCatcher ()  :  Zeubject (), ZESpatialPhagy (),
                     trig_butt_simulcount (2), trig_butt_ident (8),
                     calibrating (false), elevating (false),
                     cally (nullptr)
    { }
  ~WandCatcher () override { }


  u64 TriggerButtonIdentifier ()  const
    { return trig_butt_ident; }
  void SetTriggerButtonIdentifier (u64 tr_butt)
    { trig_butt_ident = tr_butt; }

  i32 TiggerButtonSimulcount ()  const
    { return trig_butt_simulcount; }
  void SetTriggerButtonSimulcount (i32 cnt)
    { trig_butt_simulcount = cnt; }

  // std::optional <const Vect &> RecentPos (const std::string &prov)  const
  //   { if (std::find (recentest_pos . begin (), recentest_pos . end (), prov)
  //         !=  recentest_pos . end ())
  //       return recentest_pos[prov];
  //     return {};
  //   }
  Vect AveragePos ()  const
    { Vect avg;
      i32 cnt = 0;
      for (auto &poo  :  recentest_pos)
        { avg += poo.second;  ++cnt; }
      if (cnt)  avg /= (f64)cnt;
      return avg;
    }

  ZESpatialPhagy *Calibrista ()  const
    { return cally; }
  void SetCalibrista (ZESpatialPhagy *cal)
    { cally = cal; }

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  // see below for the above... can't define inline because uses TriDemo...
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override
    { if (calibrating)
        { // avanti!
          if (cally)
            cally -> ZESpatialHarden (e);
          return 0;
        }
      if (e -> WhichPressor ()  ==  trig_butt_ident)
        { trig_partic . insert (e -> Provenance ());
          if (i64(trig_partic . size ())  >=  trig_butt_simulcount)
            calibrating = true;
          return 0;
        }

      if (e -> Aim () . Dot (Vect::yaxis)  > 0.75)
        { if (! elevating)
            { elev_partic . insert (e -> Provenance ());
              if (elev_partic . size ()  >  1)
                { elevating = true;
                  elev_prevpos = AveragePos ();
                }
            }
        }
      return 0;
    }
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override
    { if (calibrating  &&  trig_partic . size () == 0)
        { // ymlaen!
          if (cally)
            { if (cally -> ZESpatialSoften (e)  ==  -666)
                calibrating = false;
            }
          return 0;
        }
      if (e -> WhichPressor ()  ==  trig_butt_ident)
        { if (trig_partic . size ()  >  0)
            { auto it = trig_partic . find (e -> Provenance ());
              if (it  !=  trig_partic . end ())
                trig_partic . erase (it);  // three verbose lines to remove...
              if (calibrating  &&  trig_partic . size () == 0)
                fprintf (stderr, "S T A R T I N G  CALIBRATION\n");
            }
          return 0;
        }

      auto it = elev_partic . find (e -> Provenance ());
      if (it  !=  elev_partic . end ())
        { elev_partic . erase (it);
          if (elevating)
            {  elevating = false; }
        }
      return 0;
    }
};

class TriDemo final : public Application
{
 public:
  TriDemo ();
  ~TriDemo () override final;

  bool StartUp     () override final;
  bool RunOneCycle () override final;
  bool ShutDown    () override final;

  bool DoWhatThouWilt (i64 ratch, f64 thyme)  override;

  bool InitWindowingAndGraphics ();
  void ShutDownGraphics ();
  void ShutDownSceneGraph ();
  void Render ();

  void UpdateSceneGraph (i64 ratch, f64 thyme);
  void UpdateRenderLeaves (i64 ratch, f64 thyme);

  static FrameTime *GetFrameTime ();

  Layer *GetSceneLayer ();
  Layer *GetNthSceneLayer (i32 nth);
  void AppendSceneLayer (Layer *layer);
  i32 NumSceneLayers () const;

  i32 NumRenderLeaves ()  const;
  CMVTrefoil *NthRenderLeaf (i32 ind);

  i32 NumMaeses ()  const;
  PlatonicMaes *NthMaes (i32 ind);
  PlatonicMaes *FindMaesByName (const std::string &nm);

  std::vector<CMVTrefoil *> &RenderLeaves ();

  void FlatulateCursor (ZESpatialMoveEvent *e);

  void AccrueElevatorOffset (const Vect &off);

  static RawOSCWandParser &ROWP ();
  static RawMouseParser &RAMP ();

 protected:
  GLFWwindow *window;
  std::vector<Layer *> m_scene_graph_layers;

  std::vector <CMVTrefoil *> render_leaves;
  lo::Server *osc_srv;
  WandCatcher wandy;

  static RawOSCWandParser rowp;
  static RawMouseParser ramp;

 public:
  ZoftVect elev_transl;
  f64 elev_trans_mult;
};


static TriDemo *application_instance = nullptr;

RawOSCWandParser TriDemo::rowp;
RawMouseParser TriDemo::ramp;
FrameTime *s_TriDemo_frame_time = nullptr;


i64 WandCatcher::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (calibrating  &&  trig_partic . size () == 0)
    { // forward!
      if (cally)
        cally -> ZESpatialMove (e);
      return 0;
    }

  if (application_instance)
    application_instance -> FlatulateCursor (e);

  recentest_pos[e -> Provenance ()] = e -> Loc ();
  if (elevating)
    { Vect newpos = AveragePos ();
      Vect offset = newpos - elev_prevpos;
      offset = offset . Dot (Vect::yaxis) * Vect::yaxis;
      if (application_instance)
        application_instance -> AccrueElevatorOffset (offset);
      elev_prevpos = newpos;
    }

  if (PlatonicMaes *emm = application_instance -> FindMaesByName ("table"))
    { Vect p = e -> Loc ();
      p -= emm -> Loc ();
      f64 wid = emm -> Width (), hei = emm -> Height ();
      f64 d0 = 2.0 * p . Dot (emm -> Over ());
      f64 d1 = 2.0 * p . Dot (emm -> Up ());
      if (d0 < wid  &&  d0 > -wid  &&  d1 < hei  &&  d1 > -hei)
        { f64 d2 = p . Dot (emm -> Norm ());
          fprintf (stderr,
                   "YEAUH! In bounds, <%.1lf %.1lf %.1lf> surface-relative.\n",
                   d0, d1, d2);
        }
    }
  return 0;
}

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

static void glfw_key_callback(GLFWwindow *window, int key, int, int action, int)
{ if ((key == GLFW_KEY_Q  ||  key == GLFW_KEY_ESCAPE)
      &&  action == GLFW_RELEASE)
    { glfwSetWindowShouldClose (window, GLFW_TRUE);
      Application::StopRunning ();
      return;
    }
}

static void glfw_mousepos_callback (GLFWwindow *win, double x, double y)
{ CMVTrefoil *leaf = NULL;
  i32 N = application_instance -> NumMaeses ();
  for (i32 q = 0  ;  q < N  ;  ++q)
    { CMVTrefoil *l = application_instance -> NthRenderLeaf (q);
      if (x >= l->b_view->fb_pix_l  &&  x <l->b_view->fb_pix_r
          && y >= l->b_view->fb_pix_b  &&  y < l->b_view->fb_pix_t)
        { leaf = l;  break; }
    }
  if (! leaf)
    { fprintf (stderr,
               "in window %p: mouse at [%.1lf, %.1lf], but no Maes/bgfx-view\n",
               win, x, y);
      return;
    }
  x -= leaf->b_view->fb_pix_l;
  y = leaf->b_view->fb_pix_t - y - 1;   // freakin' (0,0) at the top left...
  x /= (f64)(leaf->b_view->fb_pix_r - leaf->b_view->fb_pix_l);
  y /= (f64)(leaf->b_view->fb_pix_t - leaf->b_view->fb_pix_b);

  TriDemo::RAMP().MouseMove ("mouse-0", x, y, leaf->cam);
}


static void glfw_mousebutt_callback (GLFWwindow *wind, int butt,
                                     int actn, int mods)
{
  TriDemo::RAMP().MouseButt ("mouse-0", 0x01 << butt,
                             actn == GLFW_PRESS ? 1.0 : 0.0);
}

int eruct_handler (const char *pth, const char *typ, lo_arg **av, int ac,
                   lo_message msg, void *udata)
{
  fprintf (stderr, "some other message: PTH=[%s]-TYP=[%s]-RGS[%d]\n",
           pth, typ, ac);
  return 1;
}

int osc_wandler (const char *pth, const char *typ, lo_arg **av, int ac,
                 lo_message msg, void *udata)
{
  TriDemo::ROWP() . Parse (pth, lo::Message (msg), (OmNihil *)udata);
  if (dynamic_cast <ZESpatialMoveEvent::ZESpatialMovePhage *>
      ((OmNihil *)udata))
    { int schlibble = 0;
      schlibble++;
    }
  return 0;
}

GLFWwindow *windoidal = NULL;
static i32 WINWID = 9600;
static i32 WINHEI = 2160;
bool TriDemo::InitWindowingAndGraphics ()
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
  glfwWindowHint (GLFW_DECORATED, GLFW_FALSE);

  GLFWwindow *window = glfwCreateWindow (WINWID, WINHEI, "dead zone",
                                         nullptr, nullptr);
  if (! window)
    ERROR_RETURN_VAL ("couldn't create window", false);

  windoidal = window;

  glfwSetKeyCallback (window, glfw_key_callback);
  glfwSetCursorPosCallback (window, glfw_mousepos_callback);
  glfwSetMouseButtonCallback (window, glfw_mousebutt_callback);
  //

  glfwPollEvents ();
  glfwSetWindowSize (window, WINWID, WINHEI);

  bgfx::renderFrame();
  bgfx::Init init;

  // init.callback = new DebugOutputCallbacks;
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

  for (CMVTrefoil *leaf  :  render_leaves)
    { u16 vuid = leaf->b_view -> ViewID ();
      leaf->b_view->wind_wid = glfw_width;
      leaf->b_view->wind_hei = glfw_height;
      leaf->b_view->fb_pix_r = leaf->b_view->wid_frac * glfw_width
        + (leaf->b_view->fb_pix_l = leaf->b_view->botlef_x * glfw_width);
      leaf->b_view->fb_pix_t = leaf->b_view->hei_frac * glfw_height
        + (leaf->b_view->fb_pix_b = leaf->b_view->botlef_y * glfw_height);
      bgfx::setViewRect (vuid,
                         leaf->b_view->fb_pix_l,
                         leaf->b_view->fb_pix_b,
                         leaf->b_view->wid_frac * glfw_width,
                         leaf->b_view->hei_frac * glfw_height);
      bgfx::setViewScissor (vuid,
                            leaf->b_view->fb_pix_l,
                            leaf->b_view->fb_pix_b,
                            leaf->b_view->wid_frac * glfw_width,
                            leaf->b_view->hei_frac * glfw_height);
      bgfx::setViewClear (vuid, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL,
                          0x505050FF);
      // use depth integer to order GPU draw submission
      // drawables are enumerated by order in scene graph,
      // so that id number is used as the depth.
      bgfx::setViewMode (vuid, bgfx::ViewMode::DepthAscending);
    }

  return true;
}

void TriDemo::ShutDownGraphics ()
{
  bgfx::shutdown();
  glfwTerminate();
}

void TriDemo::Render ()
{
  for (Layer *l : m_scene_graph_layers)
    for (Renderable *r : l->GetRenderables())
      r -> Update ();

  for (CMVTrefoil *leaf  :  render_leaves)
    { u16 vuid = leaf->b_view -> ViewID ();
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


TriDemo::TriDemo ()
  : window {nullptr},
    m_scene_graph_layers {new Layer},
    osc_srv (NULL), elev_trans_mult (77.0)
{ lo_server los = lo_server_new ("54345", NULL);
  if (! los)
    fprintf (stderr, "Could not conjure an OSC server -- prob'ly the port.\n");
  else
    { osc_srv = new lo::Server (los);
      //  osc_srv -> add_method ("/events/spatial", NULL, wandler, snarbs);
      osc_srv -> add_method ("/events/spatial", NULL, osc_wandler, &wandy);
      osc_srv -> add_method (NULL, NULL, eruct_handler, this);
    }

  wandy . SetCalibrista (&rowp.room_calibrex);
  elev_transl . MakeBecomeLikable ();
  application_instance = this;
}

TriDemo::~TriDemo ()
{ if (osc_srv)
    delete osc_srv;
}


bool TriDemo::StartUp ()
{
  s_TriDemo_frame_time = new FrameTime;
  bool ret = InitWindowingAndGraphics();
  VideoSystem::Initialize();

  return ret;
}

static i64 global_ratchet = 0;

bool TriDemo::RunOneCycle ()
{ int gotted;
  GetFrameTime()->UpdateTime();
  f64 global_frame_thyme = GetFrameTime () -> GetCurrentTime ();

  global_ratchet += 8;

  glfwPollEvents();
  if (osc_srv)
    while ((gotted = osc_srv -> recv (0))  >  0)
      { }  // that is to say: as long as they're there, keep 'em moving!

  if (ProtoZoftThingGuts::IsMassBreathing ())
    ProtoZoftThingGuts::MassBreather ()
      -> Inhale (global_ratchet, global_frame_thyme);

  DoWhatThouWilt (global_ratchet, global_frame_thyme);

  VideoSystem *video_system = VideoSystem::GetSystem ();
  video_system->PollMessages();
  video_system->UploadFrames();

  UpdateSceneGraph (global_ratchet, global_frame_thyme);
  UpdateRenderLeaves (global_ratchet, global_frame_thyme);

  for (CMVTrefoil *leaf  :  render_leaves)
    leaf -> Inhale (global_ratchet, global_frame_thyme);

  Render ();

  return true;
}

bool TriDemo::DoWhatThouWilt (i64, f64)
{
  return true;
}


void TriDemo::UpdateRenderLeaves (i64 ratch, f64 thyme)
{ for (CMVTrefoil *leaf  :  render_leaves)
    leaf -> Inhale (ratch, thyme);
}

void TriDemo::UpdateSceneGraph(i64 ratch, f64 thyme)
{
  graph_id id = 0;
  for (Layer *layer : m_scene_graph_layers)
    {
      layer->GetRootNode()
        ->UpdateTransformsHierarchically (ratch, thyme);
      id = layer->GetRootNode()->EnumerateRenderables(id);
    }
}

void TriDemo::ShutDownSceneGraph()
{
  for (auto *l : m_scene_graph_layers)
    delete l;

  m_scene_graph_layers.clear ();
}


bool TriDemo::ShutDown ()
{
  ShutDownSceneGraph ();
  VideoSystem::ShutDown();
  ShutDownGraphics ();

  delete s_TriDemo_frame_time;
  s_TriDemo_frame_time = nullptr;

  return true;
}

FrameTime *TriDemo::GetFrameTime ()
{
  return s_TriDemo_frame_time;
}

Layer *TriDemo::GetSceneLayer ()
{
  return m_scene_graph_layers[0];
}

Layer *TriDemo::GetNthSceneLayer (i32 nth)
{
  assert (nth < i64 (m_scene_graph_layers.size ()));
  return m_scene_graph_layers[nth];
}

void TriDemo::AppendSceneLayer (Layer *layer)
{
  if (layer == nullptr)
    return;

  m_scene_graph_layers.push_back (layer);
}

i32 TriDemo::NumSceneLayers () const
{
  return i32 (m_scene_graph_layers.size ());
}

i32 TriDemo::NumRenderLeaves ()  const
{ return render_leaves . size (); }

CMVTrefoil *TriDemo::NthRenderLeaf (i32 ind)
{ if (ind < 0  ||  ind >= render_leaves . size ())
    return NULL;
  return render_leaves[ind];
}

i32 TriDemo::NumMaeses ()  const
{ return NumRenderLeaves (); }

PlatonicMaes *TriDemo::NthMaes (i32 ind)
{ if (ind < 0  ||  ind >= render_leaves . size ())
    return NULL;
  return NthRenderLeaf (ind)->maes;
}

PlatonicMaes *TriDemo::FindMaesByName (const std::string &nm)
{ for (CMVTrefoil *leaf  :  render_leaves)
    if (leaf  &&  leaf->maes  &&  nm == leaf->maes -> Name ())
      return leaf->maes;
  return NULL;
}

std::vector<CMVTrefoil *> &TriDemo::RenderLeaves ()
{
  return render_leaves;
}


void TriDemo::FlatulateCursor (ZESpatialMoveEvent *e)
{
  // PlatonicMaes *close_m = NULL;
  // Vect close_p, hit;
  // f64 close_d;

  // const std::string &which_crs = e -> Provenance ();
  // Cursoresque *crs = NULL;
  // Cursoresque *bachelor_crs = NULL;
  // for (Cursoresque *c  :  cursoresques)
  //   { const std::string &nm = c -> Name ();
  //     if (nm . empty ())
  //       bachelor_crs = c;
  //     else if (nm  ==  which_crs)
  //       { crs = c;  break; }
  //   }
  // if (! crs)
  //   { if (! bachelor_crs)
  //       return;
  //     crs = bachelor_crs;
  //     bachelor_crs -> SetName (which_crs);
  //   }

  // i32 cnt = NumMaeses ();
  // for (i32 q = 0  ;  q < cnt  ;  ++q)
  //   { PlatonicMaes *emm = NthMaes (q);
  //     if (GeomFumble::RayRectIntersection (e -> Loc (), e -> Aim (),
  //                                          emm -> Loc (), emm -> Over (),
  //                                          emm -> Up (), emm -> Width (),
  //                                          emm -> Height (), &hit))
  //       { f64 d = hit . DistFrom (e -> Loc ());
  //         if (! close_m  ||  d < close_d)
  //           { close_m = emm;
  //             close_p = hit;
  //             close_d = d;
  //           }
  //       }
  //   }

  // if (close_m)
  //   { crs->crs_pos = close_p;
  //     crs->crs_rnd -> SetOver (close_m -> Over ());
  //     crs->crs_rnd -> SetUp (close_m -> Up ());
  //   }
}


void TriDemo::AccrueElevatorOffset (const Vect &off)
{ Vect hither = elev_transl.val + elev_trans_mult * off;
  elev_transl = hither;
}


RawOSCWandParser &TriDemo::ROWP ()
{
  return rowp;
}

RawMouseParser &TriDemo::RAMP ()
{
  return ramp;
}

int main (int, char **)
{
  TriDemo demo;

  i64 total_pixwid = 0;
  i64 total_pixhei = 0;
  i32 nm = NumMaesesFromTOML ("../configs/maes-config.toml");
  for (i32 q = 0  ;  q < nm  ;  ++q)
    if (PlatonicMaes *m = MaesFromTOML ("../configs/maes-config.toml", q))
      { CMVTrefoil *leaf = new CMVTrefoil;
        leaf->maes = m;
        Bolex *c = CameraFromMaes (*m);
        leaf->cam = c;

        if (m -> IdealPixelWidth ()  >=  0)
          total_pixwid += m -> IdealPixelWidth ();
        if (m -> IdealPixelHeight ()  >=  0)
          if (m -> IdealPixelHeight ()  >  total_pixhei)
            total_pixhei = m -> IdealPixelHeight ();

        CMVTrefoil::BGFXView *bv = new CMVTrefoil::BGFXView;
        leaf->b_view = bv;
        bv->botlef_x = 0.5 * (f64)q;
        bv->botlef_y = 0.0;
        bv->wid_frac = 0.5;
        bv->hei_frac = 1.0;
        bv->view_id = q;
        demo.RenderLeaves ().push_back (leaf);
      }

  f64 eks = 0.0;
  if (total_pixwid  >  0)
    for (CMVTrefoil *leaf  :  demo.RenderLeaves ())
      { CMVTrefoil::BGFXView *bv = leaf->b_view;
        PlatonicMaes *m = leaf->maes;
        if (m -> IdealPixelWidth ())
          { bv->wid_frac = (f64)m -> IdealPixelWidth () / (f64)total_pixwid;
            bv->botlef_x = eks;
            eks += bv->wid_frac;
          }
        if (m -> IdealPixelHeight ())
          { bv->hei_frac = (f64)m -> IdealPixelHeight () / (f64)total_pixhei;
            bv->botlef_y = 0.0;
          }
      }

  if (! demo.StartUp ())
    return -1;

  Layer *front_layer = demo.GetSceneLayer();
  Layer *left_layer = new Layer ();
  demo.AppendSceneLayer(left_layer);
  Layer *ee_layer = new Layer ();
  demo.AppendSceneLayer(ee_layer);

  for (CMVTrefoil *leaf : demo.RenderLeaves())
    {
      if (! leaf->maes)
        continue;

      if (leaf->maes->Name() == "front")
        leaf->layers.push_back(front_layer);
      else if (leaf->maes->Name () == "left")
        leaf->layers.push_back(left_layer);

      //ee_layer goes on all
      leaf->layers.push_back(ee_layer);
    }

  std::vector<FilmInfo> film_infos
    = ReadFilmInfo ("../configs/film-config.toml");
  assert (film_infos.size () > 0);

  {
    BlockTimer bt ("loading geom");
    std::vector<FilmGeometry> geoms = ReadFileGeometry("../configs/mattes.toml");
    assert (geoms.size () > 0);
    MergeFilmInfoGeometry(film_infos, geoms);
  }

  PlatonicMaes *maes = demo.FindMaesByName ("front");
  assert (maes);

  TriBand *triband = new TriBand (maes->Width (), 2.0 * maes->Height (),
                                  film_infos);
  triband->Translate (maes->Loc ());// - (1.0/2.0) * maes->Height () * maes->Up ());
  triband -> Translate (demo.elev_transl);
  front_layer->GetRootNode()->AppendChild(triband);

  PlatonicMaes *left = demo.FindMaesByName("left");
  triband = new TriBand (maes->Width (), 2.0 * maes->Height (), film_infos);
  triband->RotateD (maes->Up (), 90.0);
  triband->Translate (left->Loc ()
                      - (1.0/2.0) * left->Height () * left->Up ());
                      //+ (1.0/2.0) * maes->Height () * left->Up ());
  triband -> Translate (demo.elev_transl);
  left_layer->GetRootNode()->AppendChild(triband);

  //std::this_thread::sleep_for(std::chrono::seconds (2));

  demo.Run ();

  return 0;
}
