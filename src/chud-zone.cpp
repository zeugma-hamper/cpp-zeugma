
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

#include "GeomFumble.h"

#include "conjure-from-toml.h"

#include "vector_interop.hpp"

#include "ZESpatialEvent.h"
#include "RawEventParsing.h"

#include <lo/lo_cpp.h>

#include <bgfx_utils.hpp>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <charm_glm.hpp>

#include <string_view>

#include <unordered_set>

#include <stdio.h>

#include <Matte.hpp>


using namespace charm;


class CMVTrefoil  :  public Zeubject
{ public:
  Bolex *cam;
  PlatonicMaes *maes;
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


class dead_zone;

class Cursoresque;

// oy. also: oy.
std::vector <Cursoresque *> cursoresques;
// how about some more oy?
dead_zone *sole_dead_zone = NULL;
// don't forget: oy.
bool extra_poo = [] () { srand48 (32123);  return true; } ();

class Cursoresque  :  public Zeubject
{ public:
  Node *crs_nod;
  Renderable *crs_rnd;
  ZoftVect crs_pos;
  Cursoresque (f64 sz)  :  Zeubject (),
                           crs_nod (new Node),
                           crs_rnd (new RectangleRenderable)
    { crs_nod -> AppendRenderable (crs_rnd);
      f64 ltime = 0.55 - 0.1 * drand48 ();
      crs_nod -> Rotate (ZoftVect (Vect::zaxis),
                         LoopFloat (0.0, 2.0 * M_PI / ltime, ltime));
      crs_nod -> Scale (sz);
      crs_pos . MakeBecomeLikable ();
      crs_nod -> Translate (crs_pos);
    }
};


class WandCatcher  :  public Zeubject, public ZESpatialPhagy
{ public:
  i32 trig_butt_simulcount;
  u64 trig_butt_ident;
  std::unordered_set <std::string> trig_partic;
  bool calibrating;
  ZESpatialPhagy *cally;

  WandCatcher ()  :  Zeubject (), trig_butt_simulcount (2),
                     trig_butt_ident (8), calibrating (false), cally (NULL)
    { }


  u64 TriggerButtonIdentifier ()  const
    { return trig_butt_ident; }
  void SetTriggerButtonIdentifier (u64 tr_butt)
    { trig_butt_ident = tr_butt; }

  i32 TiggerButtonSimulcount ()  const
    { return trig_butt_simulcount; }
  void SetTriggerButtonSimulcount (i32 cnt)
    { trig_butt_simulcount = cnt; }

  ZESpatialPhagy *Calibrista ()  const
    { return cally; }
  void SetCalibrista (ZESpatialPhagy *cal)
    { cally = cal; }

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  // see below for the above... can't define inline because uses dead_zone...
  i64 ZESpatialMoveExtra (ZeEvent::ProtoSprinkler::HOOKUP const &h,
                          ZESpatialMoveEvent *e)
    { return ZESpatialMove (e); }
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override
    { if (calibrating)
        { // avanti!
          if (cally)
            cally -> ZESpatialHarden (e);
          return 0;
        }
      if (e -> WhichPressor ()  ==  trig_butt_ident)
        { trig_partic . insert (e -> Provenance ());
          if (trig_partic . size ()  >=  trig_butt_simulcount)
            calibrating = true;
          return 0;
        }
      return 0;
    }
  i64 ZESpatialHardenExtra (ZeEvent::ProtoSprinkler::HOOKUP const &h,
                            ZESpatialHardenEvent *e)
    { return ZESpatialHarden (e); }
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
      return 0;
    }
  i64 ZESpatialSoftenExtra (ZeEvent::ProtoSprinkler::HOOKUP const &h,
                            ZESpatialSoftenEvent *e)
    { return ZESpatialSoften (e); }
};



class dead_zone final : public charm::Application,
                        public ZESpatialPhagy, public Zeubject
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
  void UpdateRenderLeaves (i64 ratch, f64 thyme);

  static FrameTime *GetFrameTime ();

  Layer &GetSceneLayer ();

  i32 NumRenderLeaves ()  const;
  CMVTrefoil *NthRenderLeaf (i32 ind);

  i32 NumMaeses ()  const;
  PlatonicMaes *NthMaes (i32 ind);
  PlatonicMaes *FindMaesByName (const std::string &nm);

  void FlatulateCursor (ZESpatialMoveEvent *e);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;

 protected:
  GLFWwindow *window;
  Layer *m_scene_graph_layer;

 public:
  std::vector <CMVTrefoil *> render_leaves;
  lo::Server *osc_srv;
  WandCatcher wandy;

  static RawOSCWandParser rowp;
  static RawMouseParser ramp;
};



i64 WandCatcher::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (calibrating  &&  trig_partic . size () == 0)
    { // forward!
      if (cally)
        cally -> ZESpatialMove (e);
      return 0;
    }
  if (sole_dead_zone)
    sole_dead_zone -> FlatulateCursor (e);
  if (PlatonicMaes *emm = sole_dead_zone -> FindMaesByName ("table"))
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


RawOSCWandParser dead_zone::rowp;
RawMouseParser dead_zone::ramp;


int eruct_handler (const char *pth, const char *typ, lo_arg **av, int ac,
                   lo_message msg, void *udata)
{ fprintf (stderr, "some other message: PTH=[%s]-TYP=[%s]-RGS[%d]\n",
           pth, typ, ac);
  return 1;
}

int osc_wandler (const char *pth, const char *typ, lo_arg **av, int ac,
                 lo_message msg, void *udata)
{ dead_zone::rowp . Parse (pth, lo::Message (msg), (OmNihil *)udata);
  if (dynamic_cast <ZESpatialMoveEvent::ZESpatialMovePhage *>
      ((OmNihil *)udata))
    { int schlibble = 0;
      schlibble++;
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
  i32 N = sole_dead_zone -> NumMaeses ();
  for (i32 q = 0  ;  q < N  ;  ++q)
    { CMVTrefoil *l = sole_dead_zone -> NthRenderLeaf (q);
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

  dead_zone::ramp . MouseMove ("mouse-0", x, y, leaf->cam);//,
//                               &sole_dead_zone->wandy);
}


static void glfw_mousebutt_callback (GLFWwindow *wind, int butt,
                                     int actn, int mods)
{ dead_zone::ramp . MouseButt ("mouse-0", 0x01 << butt,
                               actn == GLFW_PRESS ? 1.0 : 0.0);//,
//                               &sole_dead_zone->wandy);
}


GLFWwindow *windoidal = NULL;
static i32 WINWID = 9600;
static i32 WINHEI = 2160;
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
  glfwWindowHint (GLFW_DECORATED, GLFW_FALSE);

  GLFWwindow *window = glfwCreateWindow (WINWID, WINHEI, "dead zone",
                                         nullptr, nullptr);
  if (! window)
    ERROR_RETURN_VAL ("couldn't create window", false);

  windoidal = window;

  glfwSetKeyCallback (window, glfw_key_callback);
  glfwSetCursorPosCallback (window, glfw_mousepos_callback);
  glfwSetMouseButtonCallback (window, glfw_mousebutt_callback);

  glfwPollEvents ();
  glfwSetWindowSize (window, WINWID, WINHEI);

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
      bgfx::setViewClear (vuid, BGFX_CLEAR_COLOR,
                          0x808080FF + vuid * 0x00200000);
      // use depth integer to order GPU draw submission
      // drawables are enumerated by order in scene graph,
      // so that id number is used as the depth.
      bgfx::setViewMode (vuid, bgfx::ViewMode::DepthAscending);
    }

  return true;
}

void dead_zone::ShutDownGraphics ()
{
  bgfx::shutdown();
  glfwTerminate();
}

void dead_zone::Render ()
{ for (Renderable *r : m_scene_graph_layer->GetRenderables())
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

      for (Renderable *r  :  m_scene_graph_layer->GetRenderables())
        r -> Draw (vuid);
    }
  bgfx::frame ();
}

FrameTime *s_dead_zone_frame_time{nullptr};

dead_zone::dead_zone ()
  : window {nullptr},
    m_scene_graph_layer {new Layer},
    osc_srv (NULL)
{ lo_server los = lo_server_new ("54345", NULL);
  if (! los)
    fprintf (stderr, "Could not conjure an OSC server -- prob'ly the port.\n");
  else
    { osc_srv = new lo::Server (los);
      //  osc_srv -> add_method ("/events/spatial", NULL, wandler, snarbs);
      osc_srv -> add_method ("/events/spatial", NULL, osc_wandler, &wandy);
      osc_srv -> add_method (NULL, NULL, eruct_handler, this);
    }

  ramp . AppendPhage (&wandy);

  wandy . SetCalibrista (&rowp.table_calibrex);
  sole_dead_zone = this;
}

dead_zone::~dead_zone ()
{ if (osc_srv)
    delete osc_srv;
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


LoopFloat elzyeff { 22.2, 3.3, 4.5 };
LoopVect elzyvee { Vect (0.0, 1.0, 2.0), Vect (1.0, -1.0, 3.0), 5.5 };
InterpFloat ayezee { -1.0, 1.0, 4.7 };
LoopFloat timey { 0.0, 1.0, 0.0 };

ZoftVect whoa_o, whoa_u;


bool dead_zone::DoWhatThouWilt (i64 ratch, f64 thyme)
{ if (ayezee.val > 0.993  &&  ayezee.val < 0.999)
    elzyvee . Restart ();

  if (timey.val > 1.5)
    { rowp . HooverCoordTransforms ();
      whoa_o = Vect::yaxis;
      whoa_u = -Vect::xaxis;
      timey . BecomeLike (ZoftFloat (0.0));
    }

  // cam -> ViewLoc () . SpewToStderr ();
  // fprintf (stderr, " is cammy's loc, and interp is all <%.2lf>\n",
  //          ayezee.val);

  return true;
}


void dead_zone::UpdateRenderLeaves (i64 ratch, f64 thyme)
{ for (CMVTrefoil *leaf  :  render_leaves)
    leaf -> Inhale (ratch, thyme);
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


i32 dead_zone::NumRenderLeaves ()  const
{ return render_leaves . size (); }

CMVTrefoil *dead_zone::NthRenderLeaf (i32 ind)
{ if (ind < 0  ||  ind >= render_leaves . size ())
    return NULL;
  return render_leaves[ind];
}

i32 dead_zone::NumMaeses ()  const
{ return NumRenderLeaves (); }

PlatonicMaes *dead_zone::NthMaes (i32 ind)
{ if (ind < 0  ||  ind >= render_leaves . size ())
    return NULL;
  return NthRenderLeaf (ind)->maes;
}

PlatonicMaes *dead_zone::FindMaesByName (const std::string &nm)
{ for (CMVTrefoil *leaf  :  render_leaves)
    if (leaf  &&  leaf->maes  &&  nm == leaf->maes -> Name ())
      return leaf->maes;
  return NULL;
}


void dead_zone::FlatulateCursor (ZESpatialMoveEvent *e)
{ PlatonicMaes *close_m = NULL;
  Vect close_p, hit;
  f64 close_d;

  const std::string &which_crs = e -> Provenance ();
  Cursoresque *crs = NULL;
  Cursoresque *bachelor_crs = NULL;
  for (Cursoresque *c  :  cursoresques)
    { const std::string &nm = c -> Name ();
      if (nm . empty ())
        bachelor_crs = c;
      else if (nm  ==  which_crs)
        { crs = c;  break; }
    }
  if (! crs)
    { if (! bachelor_crs)
        return;
      crs = bachelor_crs;
      bachelor_crs -> SetName (which_crs);
    }

  i32 cnt = NumMaeses ();
  for (i32 q = 0  ;  q < cnt  ;  ++q)
    { PlatonicMaes *emm = NthMaes (q);
      if (GeomFumble::RayRectIntersection (e -> Loc (), e -> Aim (),
                                           emm -> Loc (), emm -> Over (),
                                           emm -> Up (), emm -> Width (),
                                           emm -> Height (), &hit))
        { f64 d = hit . DistFrom (e -> Loc ());
          if (! close_m  ||  d < close_d)
            { close_m = emm;
              close_p = hit;
              close_d = d;
            }
        }
    }

  if (close_m)
    { crs->crs_pos = close_p;
      crs->crs_rnd -> SetOver (close_m -> Over ());
      crs->crs_rnd -> SetUp (close_m -> Up ());
    }
}

i64 dead_zone::ZESpatialMove (ZESpatialMoveEvent *e)
{ fprintf (stderr, "phoooooooooooot...\n");
  return 0;
}


int main (int, char **)
{
  dead_zone zone;

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
        zone.render_leaves . push_back (leaf);
      }

  f64 eks = 0.0;
  if (total_pixwid  >  0)
    for (CMVTrefoil *leaf  :  zone.render_leaves)
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

  if (! zone.StartUp ())
    return -1;

  Layer &layer = zone.GetSceneLayer();

  std::vector<FilmInfo> configs
    = ReadFilmInfo ("../configs/jh-film-config.toml");
  assert (configs.size () > 0);

  FilmInfo &film_info = configs[0];
  assert (film_info.clips.size () > 0);

  std::string uri = std::string ("file://") + film_info.film_path.c_str ();

  s_nodal = new Node ();
  VideoRenderable *renderable = new VideoRenderable (uri);
  s_nodal -> AppendRenderable (renderable);
  layer . GetRootNode () -> AppendChild (s_nodal);

  PlatonicMaes *maes = zone . FindMaesByName ("left");
  if (maes)
    { SinuVect sv (Vect (0.5, 0.0, 0.0), 1.0);
      SinuFloat sf (0.5, 0.3772, 0.0);
      s_nodal -> Rotate (ZoftVect (Vect::zaxis), sf);
      s_nodal -> Translate (sv);
      s_nodal -> RotateD (Vect (0.0, 1.0, 0.0), 90.0);
      s_nodal -> Scale (0.4 * maes -> Width ());
      s_nodal -> Translate (maes -> Loc ());
    }

  whoa_o = Vect::xaxis;
  whoa_u = Vect::yaxis;
  whoa_o . MakeBecomeLikable ();
  whoa_u . MakeBecomeLikable ();

  RectangleRenderable *rect_rend = new RectangleRenderable ();
  (dr_no = new Node) -> AppendRenderable (rect_rend);
  rect_rend -> OverZoft () . BecomeLike (whoa_o);
  rect_rend -> UpZoft () . BecomeLike (whoa_u);
  layer . GetRootNode () -> AppendChild (dr_no);

  if (maes = zone . FindMaesByName ("front"))
    { dr_no -> RotateD (Vect (0.0, 1.0, 0.0), -30.0);
      dr_no -> Scale (0.75 * maes -> Height ());
      SinuVect ss (Vect (0.25), 1.8, Vect (1.0));
      dr_no -> Scale (ss);
      dr_no -> Translate (maes -> Loc ());
    }

  RectangleRenderable *rr;
  for (int poot = 0  ;  poot < 9  ;  ++poot)
    { Node *enn = new Node;
      enn -> AppendRenderable (rr = new RectangleRenderable);
      //  enn -> Rotate (ZoftThing (Vect::zaxis), SinuFloat (0.5, 2.0));
      rr -> OverZoft () . BecomeLike (whoa_o);
      rr -> UpZoft () . BecomeLike (whoa_u);
      enn -> RotateD (Vect::yaxis, 45.0);
      enn -> Scale (0.025 * maes -> Height ());
      enn -> Translate (maes -> Loc ()
			-  0.5 * maes -> Width () * maes -> Over ()
			+  (((f64)poot * 0.125  -  0.5) * maes -> Height ()
			    * maes -> Up ()));
      layer . GetRootNode () -> AppendChild (enn);
    }

  for (int q = 0  ;  q < 3  ;  ++q)
    { Cursoresque *c = new Cursoresque (0.015 * maes -> Height ());
      layer . GetRootNode () -> AppendChild (c->crs_nod);

      cursoresques . push_back (c);
      c->crs_pos = maes -> Loc ();
    }

  zone.Run ();

  return 0;
}
