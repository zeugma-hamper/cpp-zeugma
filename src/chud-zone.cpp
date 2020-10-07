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


class Whippletree  :  public Zeubject
{ public:
  Bolex *cam;
  PlatonicMaes *maes;
  struct BGFXView {
    i32 view_id;  // bgfx's is u16, so we can rep't stuff in neg nos.
    f64 botlef_x, botlef_y;
    f64 wid_frac, hei_frac;  // these four norm'd [0,1] ref'ing window res
    i32 fb_pix_w, fb_pix_h;
    u16 ViewID ()  const  { return (u16)view_id; }
  } *b_view;
  Whippletree ()  :  Zeubject (), cam (NULL), maes (NULL), b_view (NULL)
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

  cam -> SetViewHorizAngle (2.0 * asin (0.5 * m . Width () / dst));
  cam -> SetViewVertAngle (2.0 * asin (0.5 * m . Height () / dst));

  cam -> SetNearAndFarClipDist (0.1, 10.0 * dst);
  return cam;
}



class WandCatcher  :  public Zeubject, public ZESpatialPhagy
{ public:
  Vect just_now, accum;
  Vect vee[3];
  int phase, count;
  std::unordered_set <std::string> trig_partic;
  bool calibrating;
  bool slurping;
  f64 above_the_floor_ness, lateral_stretchy_metron;
  Matrix44 geo_truth;

  WandCatcher ()  :  Zeubject ()
    { Reset (); }

  void Reset ()
    { above_the_floor_ness = 653.0;
      lateral_stretchy_metron = 4000.0;
      phase = -1;
      calibrating = slurping = false;
      accum . Zero ();
      vee[0] . Zero ();  vee[1] . Zero ();  vee[2] . Zero ();
    }

  void SummonTheDemiurgeCalculon (Matrix44 &outmat)
    { Vect e0 = vee[1] - vee[0];
      f64 d = e0 . NormSelfReturningMag ();
      Vect e1 = e0 . Cross (vee[2] - vee[0]) . Norm ();
      Vect e2 = e0 . Cross (e1);

      Matrix44 ma (INITLESS);
      ma . LoadTranslation (-e0);
      Matrix44 mb;
      mb.m[0][0] = e0.x;  mb.m[0][1] = e1.x;  mb.m[0][2] = e2.x;
      mb.m[1][0] = e0.y;  mb.m[1][1] = e1.y;  mb.m[1][2] = e2.y;
      mb.m[2][0] = e0.z;  mb.m[2][1] = e1.z;  mb.m[2][2] = e2.z;
      Matrix44 mc (INITLESS);
      mc . LoadScale (lateral_stretchy_metron / d);
      Matrix44 md (INITLESS);
      md . LoadTranslation (-0.5 * d, above_the_floor_ness, 0.0);

      outmat = ma * mb * mc * md;
    }

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override
    { if (! calibrating  ||  trig_partic . size () > 0)
        return 0;
      if (e -> Provenance ()  !=  "wand-0")
        return 0;
      just_now = e -> Loc ();
      if (! slurping)
        { just_now . SpewToStderr ();  fprintf (stderr, "\n");
          return 0;
        }
      accum += just_now;
      ++count;
      fprintf (stderr, "ACCUM [phase %d, count %d]: ", phase, count);
      just_now . SpewToStderr ();
      return 0;
    }
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override
    { if (! calibrating)
        { if (e -> WhichPressor ()  !=  8)
            return 0;
          trig_partic . insert (e -> Provenance ());
          if (trig_partic . size ()  >  1)
            calibrating = true;
          return 0;
        }
      if (e -> Provenance ()  ==  "wand-0")
        return 0;
      slurping = true;
      accum = just_now;
      count = 1;  // accum already has the most recent, see? already ahead!
      ++phase;
      fprintf (stderr, "\n\n\n--- COLLECTION PHASE %d%d%d ---\n", phase,
               phase, phase);
      return 0;
    }
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override
    { if (trig_partic . size ()  >  0)
        { if (e -> WhichPressor ()  ==  8)
            { auto it = trig_partic . find (e -> Provenance ());
              if (it  !=  trig_partic . end ())
                trig_partic . erase (it);
            }
          return 0;
        }
      if (! calibrating)  // cain't hardly git here nohow...
        return 0;
      if (e -> Provenance ()  ==  "wand-0")
        return 0;
      vee[phase] = accum / (f64)count;
      slurping = false;
      if (phase < 2)
        return 0;
      SummonTheDemiurgeCalculon (geo_truth);
      phase = -1;
      calibrating = false;
      return 0;
    }
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

  i32 NumMaeses ()  const;
  PlatonicMaes *NthMaes (i32 ind);
  PlatonicMaes *FindMaesByName (const std::string &nm);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;

 protected:

//  Bolex *cam;

  GLFWwindow *window;

  Layer *m_scene_graph_layer;

 public:
  std::vector <Whippletree *> render_leaves;
  lo::Server *osc_srv;
  WandCatcher wandy;

  static RawOSCWandParser rowp;
};


RawOSCWandParser dead_zone::rowp;


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

//void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
static void glfw_key_callback(GLFWwindow *window, int key, int, int action, int)
{ if ((key == GLFW_KEY_Q  ||  key == GLFW_KEY_ESCAPE)
      &&  action == GLFW_RELEASE)
    { glfwSetWindowShouldClose (window, GLFW_TRUE);
      Application::StopRunning ();
      return;
    }
}

static i32 WINWID = 7680;
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

  glfwSetKeyCallback(window, glfw_key_callback);

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

  for (Whippletree *leaf  :  render_leaves)
    { u16 vuid = leaf->b_view -> ViewID ();
      leaf->b_view->fb_pix_w = glfw_width;
      leaf->b_view->fb_pix_h = glfw_height;
      bgfx::setViewRect (vuid,
                         leaf->b_view->botlef_x * glfw_width,
                         leaf->b_view->botlef_y * glfw_height,
                         leaf->b_view->wid_frac * glfw_width,
                         leaf->b_view->hei_frac * glfw_height);
      bgfx::setViewScissor (vuid,
                            leaf->b_view->botlef_x * glfw_width,
                            leaf->b_view->botlef_y * glfw_height,
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

  for (Whippletree *leaf  :  render_leaves)
    { u16 vuid = leaf->b_view -> ViewID ();
      bgfx::touch (vuid);

      Bolex *c = leaf->cam;
      glm::mat4 view_transform = glm::lookAt (as_glm (c -> ViewLoc ()),
                                              as_glm (c -> ViewCOI ()),
                                              as_glm (c -> ViewUp ()));
      glm::mat4 proj_transform
        = glm::perspective ((float) c -> ViewVertAngle (),
                            (float) (sin (0.5 * c -> ViewHorizAngle ())
                                     / sin (0.5 * c -> ViewVertAngle ())),
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

//  cam -> Inhale (global_ratchet, global_frame_thyme);

  Render ();

  return true;
}


LoopFloat elzyeff { 22.2, 3.3, 4.5 };
LoopVect elzyvee { Vect (0.0, 1.0, 2.0), Vect (1.0, -1.0, 3.0), 5.5 };
InterpFloat ayezee { -1.0, 1.0, 4.7 };

bool dead_zone::DoWhatThouWilt (i64 ratch, f64 thyme)
{ if (ayezee.val > 0.993  &&  ayezee.val < 0.999)
    elzyvee . Restart ();

  // cam -> ViewLoc () . SpewToStderr ();
  // fprintf (stderr, " is cammy's loc, and interp is all <%.2lf>\n",
  //          ayezee.val);

  return true;
}


void dead_zone::UpdateRenderLeaves (i64 ratch, f64 thyme)
{ for (Whippletree *leaf  :  render_leaves)
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


i32 dead_zone::NumMaeses ()  const
{ return render_leaves . size (); }

PlatonicMaes *dead_zone::NthMaes (i32 ind)
{ if (ind < 0  ||  ind >= render_leaves . size ())
    return NULL;
  return render_leaves[ind]->maes;
}

PlatonicMaes *dead_zone::FindMaesByName (const std::string &nm)
{ for (Whippletree *leaf  :  render_leaves)
    if (leaf  &&  leaf->maes  &&  nm == leaf->maes -> Name ())
      return leaf->maes;
  return NULL;
}


i64 dead_zone::ZESpatialMove (ZESpatialMoveEvent *e)
{ fprintf (stderr, "phoooooooooooot...\n");
  return 0;
}


int main (int, char **)
{
  dead_zone zone;

  i32 nm = NumMaesesFromTOML ("../maes-config.toml");
  for (i32 q = 0  ;  q < nm  ;  ++q)
    if (PlatonicMaes *m = MaesFromTOML ("../maes-config.toml", q))
      { Whippletree *leaf = new Whippletree;
        leaf->maes = m;
        Bolex *c = CameraFromMaes (*m);
        leaf->cam = c;
        Whippletree::BGFXView *bv = new Whippletree::BGFXView;
        leaf->b_view = bv;
        bv->botlef_x = 0.5 * (f64)q;
        bv->botlef_y = 0.0;
        bv->wid_frac = 0.5;
        bv->hei_frac = 1.0;
        bv->view_id = q;
        zone.render_leaves . push_back (leaf);
      }

  if (! zone.StartUp ())
    return -1;

  Layer &layer = zone.GetSceneLayer();

  std::vector<FilmInfo> configs = ReadFilmInfo ("../jh-film-config.toml");
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

  RectangleRenderable *rect_rend = new RectangleRenderable ();
  (dr_no = new Node) -> AppendRenderable (rect_rend);
  layer . GetRootNode () -> AppendChild (dr_no);

  if (maes = zone . FindMaesByName ("front"))
    { dr_no -> RotateD (Vect (0.0, 1.0, 0.0), -30.0);
      dr_no -> Scale (1.0 * maes -> Height ());
      SinuVect ss (Vect (0.1), 1.8, Vect (1.0));
      dr_no -> Scale (ss);
      dr_no -> Translate (maes -> Loc ());
    }

  for (int poot = 0  ;  poot < 9  ;  ++poot)
    { Node *enn = new Node;
      enn -> AppendRenderable (new RectangleRenderable);
      //  enn -> Rotate (ZoftThing (Vect::zaxis), SinuFloat (0.5, 2.0));
      enn -> RotateD (Vect::yaxis, 45.0);
      enn -> Scale (0.025 * maes -> Height ());
      enn -> Translate (maes -> Loc ()
			-  0.5 * maes -> Width () * maes -> Over ()
			+  (((f64)poot * 0.125  -  0.5) * maes -> Height ()
			    * maes -> Up ()));
      layer . GetRootNode () -> AppendChild (enn);
    }

  zone.Run ();

  return 0;
}
