
//application
#include <GraphicsApplication.hpp>
#include <FrameTime.hpp>
#include <Layer.hpp>

//video, mattes
#include <VideoSystem.hpp>
#include <Matte.hpp>

//scene graph
#include <Node.hpp>
#include <Frontier.hpp>

//Renderables
#include <Renderable.hpp>
#include <RectangleRenderable.hpp>
#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>
#include <TexturedRenderable.hpp>
#include <LinePileRenderable.h>
#include <PolygonRenderable.h>
#include <GridRenderable.h>

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
#include <LoopZoft.h>
#include "SinuZoft.h"
#include "SumZoft.h"
#include "RandZoft.h"
#include "TrGrappler.h"
#include "ScGrappler.h"

//events
#include <GLFWWaterWorks.hpp>
#include <ZEBulletinEvent.h>
#include <ZESpatialEvent.h>
#include <ZEYowlEvent.h>
#include <RawEventParsing.h>
#include <GeomFumble.h>

// audio
#include "AudioMessenger.hpp"

//UI
#include "Alignifer.h"

//etc.
#include <BlockTimer.hpp>

//dependencies
#include <bgfx_utils.hpp>

#include <charm_glm.hpp>
#include <vector_interop.hpp>

//C++, C
#include <unordered_set>
#include <boost/program_options.hpp>

#include <stdio.h>

#include <iostream>

// it's us! right here!
#include "Orksur.h"
#include "Ticato.h"
#include "AtomicFreezone.h"
#include "OeuvreAfterlife.h"
#include "GraumanPalace.h"
#include "tamparams.h"


using namespace charm;


// don't forget: oy.
bool extra_poo = [] () { srand48 (32123);  return true; } ();


class Cursoresque  :  public Alignifer
{ public:
  PolygonRenderable *re1,  *re2;
  PlatonicMaes *cur_maes;
  Cursoresque (f64 sz, i64 nv = 6)  :  Alignifer (),
                                       re1 (new PolygonRenderable),
                                       re2 (new PolygonRenderable),
                                       cur_maes (NULL)
    { AppendRenderable (re1);
      AppendRenderable (re2);
      re1 -> SetFillColor (ZeColor (1.0, 0.5));
      re2 -> SetFillColor (ZeColor (1.0, 0.5));
      for (i64 w = 0  ;  w < 2  ;  ++w)
        for (i64 q = 0  ;  q < nv  ;  ++q)
          { f64 theeeta = 2.0 * M_PI / (f64)nv * (f64)q  +  w * M_PI;
            Vect radial = (0.5 * (w + 1.0))
              *  (cos (theeeta) * Vect::xaxis  +  sin (theeeta) * Vect::yaxis);
            SinuVect arm (0.065 * radial, 0.8 + 0.11 * drand48 (),
                          0.24 * (1.0 + 3.0 * (q%2)) * radial);
            (w > 0 ? re1 : re2) -> AppendVertex (arm);
          }
      ScaleZoft () = Vect (sz);
    }
  void Invisify ()
    { re1 -> SetShouldDraw (false);  re2 -> SetShouldDraw (false); }
  void Visibloy ()
    { re1 -> SetShouldDraw (true);  re2 -> SetShouldDraw (true); }
};


std::vector <Cursoresque *> cursoresques;


class Sensorium  :  public Zeubject, public ZESpatialPhagy, public ZEYowlPhagy
{ public:
  i32 trig_butt_simulcount;
  u64 trig_butt_ident;
  std::unordered_set <std::string> trig_partic;
  std::unordered_set <std::string> elev_partic;
  bool calibrating;
  bool elevating;
  Vect elev_prevpos;
  std::map <std::string, Vect> recentest_pos;
  ZESpatialPhagy *cally;

  Sensorium ()  :  Zeubject (), ZESpatialPhagy (),
                   trig_butt_simulcount (2), trig_butt_ident (8),
                   calibrating (false), elevating (false),
                   cally (nullptr)
    { }
  ~Sensorium () override { }


  u64 TriggerButtonIdentifier ()  const
    { return trig_butt_ident; }
  void SetTriggerButtonIdentifier (u64 tr_butt)
    { trig_butt_ident = tr_butt; }

  i32 TiggerButtonSimulcount ()  const
    { return trig_butt_simulcount; }
  void SetTriggerButtonSimulcount (i32 cnt)
    { trig_butt_simulcount = cnt; }

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
  // see below for the above... can't define inline because uses Tampo...
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;

  i64 ZEYowlAppear (ZEYowlAppearEvent *e)  override;
};


class Tampo final : public GraphicsApplication
{ public:
  Tampo ();
  ~Tampo () override final;

  bool DoWhatThouWilt (i64 ratch, f64 thyme)  override;

  Frontier *IntersectedFrontier (const Vect &frm, const Vect &aim,
                                 Vect *hit_point = NULL);
  void FlatulateCursor (ZESpatialMoveEvent *e);

  void AccrueElevatorOffset (const Vect &off);

 public:
  InterpVect elev_transl;
  f64 elev_trans_mult;
  VideoRenderable *steenbeck;
  ch_ptr <AtomicFreezone> freezo;
  ch_ptr <OeuvreAfterlife> vreaft;
  ch_ptr <Orksur> orksu;
  ch_ptr<Sensorium> sensy;
  ch_ptr<GraumanPalace> gegyp;
  ch_ptr<GraumanPalace> gchin;
  Node *texxyno;
};


static Tampo *solo_tamp = NULL;



i64 Sensorium::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (calibrating  &&  trig_partic . size () == 0)
    { // forward!
      if (cally)
        cally -> ZESpatialMove (e);
      return 0;
    }

  Tampo *tam = (Tampo *)GraphicsApplication::GetApplication ();
  tam -> FlatulateCursor (e);

  recentest_pos[e -> Provenance ()] = e -> Loc ();

/*   // not permitting the ui-driven elevator thing for now
  if (elevating)
    { Vect newpos = AveragePos ();
      Vect offset = newpos - elev_prevpos;
      offset = offset . Dot (Vect::yaxis) * Vect::yaxis;
      if (tam)
        tam -> AccrueElevatorOffset (offset);
      elev_prevpos = newpos;
    }
  else
    { }
*/
  if (PlatonicMaes *emm = tam -> FindMaesByName ("table"))
    { Vect p = e -> Loc ();
      p -= emm -> Loc ();
      f64 wid = emm -> Width (), hei = emm -> Height ();
      f64 d0 = 2.0 * p . Dot (emm -> Over ());
      f64 d1 = 2.0 * p . Dot (emm -> Up ());
      if (d0 < wid  &&  d0 > -wid  &&  d1 < hei  &&  d1 > -hei)
        { f64 d2 = p . Dot (emm -> Norm ());
          // fprintf (stderr,
          //          "YEAUH! In bounds, <%.1lf %.1lf %.1lf> surface-relative.\n",
          //          d0, d1, d2);
        }
    }
  return 0;
}

i64 Sensorium::ZESpatialHarden (ZESpatialHardenEvent *e)
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

  Tampo *instance = (Tampo *)GraphicsApplication::GetApplication();
  Vect hit;
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

i64 Sensorium::ZESpatialSoften (ZESpatialSoftenEvent *e)
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
/*  // no ui-driven elevation for the nonce...
  auto it = elev_partic . find (e -> Provenance ());
  if (it  !=  elev_partic . end ())
    { elev_partic . erase (it);
      if (elevating)
        {  elevating = false; }
    }
*/
  return 0;
}


i64 Sensorium::ZEYowlAppear (ZEYowlAppearEvent *e)
{ const std::string &utt = e -> Utterance ();
  if (utt == "q")
    { InterpVect &rs = Tamglobals::Only ()->room_scaler;
      rs . Reverse ();
      rs . Commence ();
      bool &oto = Tamglobals::Only ()->room_is_scaled_oto;
      if (oto)
        solo_tamp->elev_transl . Set (Vect::zerov);
      else
        solo_tamp->elev_transl
          . Set (Vect (0.0, Tamglobals::Only ()->cur_elev_stop, 0.0));
      oto = ! oto;
    }
  else if (utt == "w"  |  utt == "e"  |  utt == "r")
    { bool &oto = Tamglobals::Only ()->room_is_scaled_oto;
      f64 nes = (utt == "w"  ?  Tamparams::Current ()->workband_elevstop
                 :  (utt == "e"  ?  Tamparams::Current ()->escband_elevstop
                     :  Tamparams::Current ()->collabband_elevstop));
      if (nes  !=  Tamglobals::Only ()->cur_elev_stop)
        { Tamglobals::Only ()->cur_elev_stop = -nes;
          if (oto)
            solo_tamp->elev_transl . Set (Vect (0.0, -nes, 0.0));
        }
    }
  else if (utt  ==  "t")
    { static bool cur_vis = false;
      cur_vis = ! cur_vis;
      // for (Node *no  :  Tamglobals::Only ()->construction_marks)
      //   if (no)
      //     no -> SetVisibilityForAllLocalRenderables (cur_vis);
      Tamglobals::Only ()->construction_marks_color . Reverse ();
      Tamglobals::Only ()->construction_marks_color . Commence ();
    }
  return 0;
}

#define ERROR_RETURN_VAL(MSG, VAL)                 \
  { fprintf (stderr, "%s\n", MSG);                 \
    return VAL;                                    \
  }

#define ERROR_RETURN(MSG)                          \
  { fprintf (stderr, "%s\n", MSG);                 \
    return;                                        \
  }


Tampo::Tampo ()  :  GraphicsApplication (),
                    sensy (new Sensorium),
                    elev_trans_mult (77.0),
                    steenbeck (NULL),
                    texxyno (NULL)
{ AppendSpatialPhage (&m_event_sprinkler, sensy);
  AppendYowlPhage (&m_event_sprinkler, sensy);
  elev_transl . SetInterpFunc (InterpFuncs::CUBIC_AB);

}


Tampo::~Tampo ()
{ }


bool Tampo::DoWhatThouWilt (i64 ratch, f64 thyme)
{ static MotherTime runt;
  static i64 farr = ratch;
  static LoopFloat timey { 0.0, 1.0, 0.0 };

  if (runt . DeltaTimeGlance ()  >=  5.0)
    { fprintf (stderr, "%.1lf fps...\n",
               f64((ratch - farr) / 8) / runt . DeltaTime ());
      farr = ratch;
    }

  if (texxyno  &&  timey.val > 8.5)
    { if (Node *par = texxyno -> Parent ())
        { par -> ExciseChild (texxyno);
          delete texxyno;
          texxyno = NULL;
        }
    }

  if (orksu)
    orksu -> Inhale (ratch, thyme);

  if (gegyp)
    gegyp -> Inhale (ratch, thyme);

  if (freezo)
    freezo -> Inhale (ratch, thyme);

  if (vreaft)
    vreaft -> Inhale (ratch, thyme);

  return true;
}

Frontier *Tampo::IntersectedFrontier (const Vect &frm, const Vect &aim,
                                      Vect *hit_point)
{ Vect hit;
  for (Layer *l  :  m_scene_graph_layers)
    { Frontier *f = l -> FirstHitFrontier ({frm, aim}, &hit);
      if (f)
        { fprintf (stdout, "I hit %p at (%0.2f, %0.2f, %0.2f)\n", f, hit.x, hit.y, hit.z);
          if (hit_point)
            *hit_point = hit;
          return f;
        }
    }
  return NULL;
}


void Tampo::FlatulateCursor (ZESpatialMoveEvent *e)
{ const std::string &which_crs = e -> Provenance ();
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

  Vect hit;
  if (PlatonicMaes *emm = ClosestIntersectedMaes (e -> Loc (), e -> Aim (),
                                                  &hit))
    { crs -> LocZoft () . Set (hit);
      if (emm  !=  crs->cur_maes)
        { crs -> AlignToMaes (emm);
          if (emm -> Name ()  ==  "table")
            crs -> Invisify ();
          else if (crs->cur_maes  &&  crs->cur_maes -> Name ()  ==  "table")
            crs -> Visibloy ();
          crs->cur_maes = emm;
        }
    }
}


void Tampo::AccrueElevatorOffset (const Vect &off)
{ Vect hither = elev_transl.val + elev_trans_mult * off;
  elev_transl = hither;
}


extern bool json_is_not_insanely_wrapped;


namespace po = boost::program_options;


int main (int ac, char **av)
{ // well, here we go.
  std::string aud_host ("sepsis.local");
  std::string aud_port ("57121");

 po::options_description desc ("available options");
  desc . add_options ()
    ("help", "hm. what do you think?")
    ("audserver-host", po::value <std::string> (&aud_host),
     "host, via ip or name, where audio server dwells")
    ("audserver-port", po::value <std::string> (&aud_port),
     "port on which audio server listens (default: 57121)")
    ("sane-json", "json slurped from liblo isn't wrapped in an array")
/*    ("prison-break", "escaped elements only mode")
    ("clip-collages", "disallow collage elements outside rect-bounds")
    ("background-gray", po::value<f64>(&global_param_background_gray),
     "gray value (range 0.0-1.0) for screen background; default is 0.0")
    ("ee-scale", po::value<f64>(&global_param_ee_scale),
     "median scale for escaped elements; default is 0.5")
    ("ee-scale-delta", po::value<f64>(&global_param_ee_scale_delta),
     "+/- scale range for escaped elements (around median); default is 0.0")
    ("ee-count-per-wall", po::value<i32>(&global_param_ee_count_per_wall),
     "how many escaped elements per wall; default 10")*/
    ;

  po::variables_map arg_map;
  po::store (po::parse_command_line (ac, av, desc), arg_map);
  po::notify (arg_map);


  if (arg_map . count ("help"))
    { std::cout << desc << "\n";
      return 0;
    }

  if (arg_map . count ("sane-json"))
    json_is_not_insanely_wrapped = true;
/*
  if (arg_map.count ("prison-break"))
    global_params_prison_break_mode = true;
  if (arg_map . count ("clip-collages"))
    global_param_should_clip_collages = true;
*/
  Tampo tamp;
  solo_tamp = &tamp;

  if (! tamp . StartUp ())
    return -1;

  Tamparams::UpdateViaTOMLFile ("../configs/seasonings.toml");
fprintf(stderr,"harumph: <%s>\n", Tamparams::Current ()->asc_table_slide_audio.c_str());

  AudioMessenger *a_mess = new AudioMessenger (aud_host, aud_port);
  Tamglobals::Only ()->sono_hermes = a_mess;

  TASReceiver *audio_rejoinder_catcher = new TASReceiver ("57122");
  tamp . AppendWaterWorks (audio_rejoinder_catcher);

  Layer *omni_layer = tamp . GetSceneLayer();
  Layer *front_layer = new Layer ();
  tamp . AppendSceneLayer (front_layer);
  Layer *left_layer = new Layer ();
  tamp . AppendSceneLayer (left_layer);
  Layer *walls_layer = new Layer ();
  tamp . AppendSceneLayer (walls_layer);
  Layer *table_layer = new Layer ();
  tamp . AppendSceneLayer (table_layer);

  i32 const leaf_count = tamp . NumRenderLeaves();
  for (i32 i = 0; i < leaf_count; ++i)
    { CMVTrefoil *leaf = tamp . NthRenderLeaf(i);
      if (! leaf->maes)
        continue;

      if (leaf->maes -> Name ()  ==  "front")
        { leaf->layers . push_back (front_layer);
          leaf->layers . push_back (walls_layer);
        }
      if (leaf->maes -> Name ()  ==  "left")
        { leaf->layers . push_back (left_layer);
          leaf->layers . push_back (walls_layer);
        }
      if (leaf->maes -> Name ()  ==  "table")
        { leaf->layers . push_back (table_layer);
          leaf -> SetSyntheticSpatialEventDist (100.0);
        }
      //ee_layer goes on all
      leaf->layers . push_back (omni_layer);
    }

  PlatonicMaes *frnt = tamp . FindMaesByName ("front");
  assert (frnt);
  PlatonicMaes *left = tamp . FindMaesByName ("left");
  assert (left);
  PlatonicMaes *tabl = tamp . FindMaesByName ("table");
  assert (tabl);

  // front_layer -> AppendRenderTargetMaes (frnt);
  // left_layer -> AppendRenderTargetMaes (left);
  // table_layer -> AppendRenderTargetMaes (tabl);

  f64 const total_height = 2.0 * frnt -> Height();
  f64 const total_width = frnt -> Width ();
  f64 const band_height = total_height / 3.0;

  Node *g_front_wall = new Node;
  g_front_wall -> Translate (tamp . elev_transl);
  front_layer -> GetRootNode () -> AppendChild (g_front_wall);
  Tamglobals::Only ()->front_wall = g_front_wall;

  Node *g_left_wall = new Node;
  g_left_wall -> Translate (tamp . elev_transl);
  left_layer -> GetRootNode () -> AppendChild (g_left_wall);
  Tamglobals::Only ()->left_wall = g_left_wall;

  Node *g_wallpaper = new Node;
  g_wallpaper -> Translate (tamp . elev_transl);
  walls_layer -> GetRootNode () -> AppendChild (g_wallpaper);
  Tamglobals::Only ()->wallpaper = g_wallpaper;

  Node *g_tablecloth = new Node;
  table_layer -> GetRootNode () -> AppendChild (g_tablecloth);
  Tamglobals::Only ()->tablecloth = g_tablecloth;

  Node *g_conveyor = new Node;
  omni_layer -> GetRootNode () -> AppendChild (g_conveyor);
  Tamglobals::Only ()->conveyor = g_conveyor;

  Node *g_windshield = new Node;
  omni_layer -> GetRootNode () -> AppendChild (g_windshield);
  Tamglobals::Only ()->windshield = g_windshield;

  if (PlatonicMaes *mae = tamp . FindMaesByName ("front"))
    { InterpVect &rs = Tamglobals::Only ()->room_scaler;
      rs . SetInterpFunc (InterpFuncs::CUBIC_AB);
      f64 rmf = Tamparams::Current ()->room_minify_factor;
      rs . PointA () . Set (Vect (rmf, rmf, rmf));
      rs . PointB () . Set (Vect::onesv);
      rs . Finish ();

      ScGrappler *scg = new ScGrappler (rs, ZoftVect (mae -> CornerBL ()));
      scg -> SetName ("room-scaler");
      g_wallpaper -> AppendGrappler (scg);

      scg = new ScGrappler (rs, ZoftVect (mae -> CornerBL ()));
      scg -> SetName ("room-scaler");
      g_front_wall -> AppendGrappler (scg);

      scg = new ScGrappler (rs, ZoftVect (mae -> CornerBL ()));
      scg -> SetName ("room-scaler");
      g_left_wall -> AppendGrappler (scg);
    }

  Vect auth_left_cntr
    = (-0.5 * (frnt -> Width () - left -> Width ()) * left -> Over ()
       -  0.5 * frnt -> Width () * frnt -> Over ()
       +  frnt -> Loc () . Dot (frnt -> Up ()) * frnt -> Up ());

  for (int q = 0  ;  q < 3  ;  ++q)
    { Cursoresque *c = new Cursoresque (0.015 * frnt -> Height ());
      g_windshield -> AppendChild (c);

      cursoresques . push_back (c);
      c -> LocZoft () = frnt -> Loc ();
    }

GridRenderable *griddy = new GridRenderable;
Node *gridno = new Node (griddy);
griddy -> SetCenter (tabl -> Loc ());
griddy -> SetOver (tabl -> Over ());
griddy -> SetUp (tabl -> Up ());
griddy -> SetWidth (/*0.5 * */tabl -> Width ());
griddy -> SetHeight (/*0.5 * */tabl -> Height ());
griddy -> SetWarp (0.025 * tabl -> Over ());
griddy -> SetWeft (0.025 * tabl -> Up ());
/*
griddy -> SetDiscRadius (220.0);
griddy -> SetDiscCenter (tabl -> Loc () + 150.0 * tabl -> Up ()
                         + 50.0 * tabl -> Norm ());
griddy -> SpanFractionZoft () . BecomeLike (SinuFloat (0.2, 0.4, 0.5));
*/
griddy -> SetGridColor (ZeColor (1.0, 1.0, 1.0, 0.075));
g_tablecloth -> AppendChild (gridno);

  Orksur *orkp = new Orksur (*tabl);
  tamp.orksu = ch_ptr <Orksur> (orkp);
  orkp->associated_wallmaes = frnt;
  g_tablecloth -> AppendChild (orkp);
  tamp . GetSprinkler () . AppendPhage <TASSuggestionEvent> (tamp.orksu);

  for (i64 q = 0  ;  q < tamp . NumWaterWorkses ()  ;  ++q)
    if (GLFWWaterWorks *ww
        = dynamic_cast <GLFWWaterWorks *> (tamp . NthWaterWorks (q)))
      ww -> SetPromoteMouseToSpatialOrthoStyle (true);


  std::vector <FilmInfo> film_infos
    = ReadFilmInfo ("../configs/film-config.toml");
  assert (film_infos.size () > 0);

  { BlockTimer bt ("loading & merging geom");
    BlockTimer slurpt ("slurping geom file");
    std::vector <FilmGeometry> geoms
      = ReadFileGeometry("../configs/mattes.toml");
    slurpt . StopTimer ();
    assert (geoms . size ()  >  0);
    MergeFilmInfoGeometry (film_infos, geoms);
  }

  Vect cine_cnt = frnt -> Loc ();
  cine_cnt -= cine_cnt . Dot (frnt -> Up ()) * frnt -> Up ();
  cine_cnt += Tamparams::Current ()->workband_mid * frnt -> Up ();

  GraumanPalace *grau_egyp = new GraumanPalace;
  grau_egyp -> ImportExhibitionRoster (film_infos);
  grau_egyp -> JumpToFlick (0);
  grau_egyp -> SetBackingMaes (frnt);
  (grau_egyp -> Translate (cine_cnt)) -> SetName ("transly");
  g_front_wall -> AppendChild (grau_egyp);

  tamp.gegyp = ch_ptr <GraumanPalace> (grau_egyp);

  cine_cnt = auth_left_cntr;
  cine_cnt -= cine_cnt . Dot (left -> Up ()) * left -> Up ();
  cine_cnt += Tamparams::Current ()->workband_mid * left -> Up ();

  GraumanPalace *grau_chin = new GraumanPalace;
  grau_chin -> SetOverUp (left -> Over (), left -> Up ());
  grau_chin -> ImportExhibitionRoster (film_infos);
  (grau_chin -> Translate (cine_cnt)) -> SetName ("transly");
  drand48 ();
  grau_chin -> ReleasePushback ();
//  grau_chin -> JumpToRandomFlick ();
  grau_chin -> JumpToFlick (11);
  grau_chin -> SetBackingMaes (left);
  g_left_wall -> AppendChild (grau_chin);

  tamp.gchin = ch_ptr <GraumanPalace> (grau_chin);

/*
  Node *splat = new Node;
  PolygonRenderable *polysplat = new PolygonRenderable;
  i64 nv = 14;
  SinuVect centz (1000.0 * frnt -> Over (), 0.05, frnt -> Loc ());
  for (i64 q = 0  ;  q < nv  ;  ++q)
    { f64 r = 0.3 * frnt -> Height ();
      f64 theeeta = 2.0 * M_PI / (f64)nv * (f64)q;
      Vect radv = cos (theeeta) * Vect::xaxis  +  sin (theeeta) * Vect::yaxis;
      SinuVect arm (0.3 * r * radv, 0.166 + 0.05 * drand48 (),
                    0.5 * (1.0 + q%2) * r * radv);
      SumVect voit (arm, centz);
      polysplat -> AppendVertex (voit);
    }
  polysplat -> SetFillColor (SinuZoft (ZeColor (0.0, 0.0, 0.5, 0.3), 0.16,
                                       ZeColor (1.0, 1.0, 0.0, 0.3)));
  splat -> AppendRenderable (polysplat);
  polysplat -> SetShouldEdge (true);
//  polysplat -> SetShouldFill (false);
//  polysplat -> SetShouldClose (false);
  g_windshield -> AppendChild (splat);
*/

/*
  splat = new Node (polysplat = new PolygonRenderable);
  f64 jiglet = 0.025 * frnt -> Height ();
  SinuVect jigamp (Vect (jiglet, jiglet, 0.0), 0.2975);
  for (i64 q = 0  ;  q < 12  ;  ++q)
    { f64 r = 0.3 * frnt -> Height ();
      f64 theeeta = 0.5 * M_PI * (f64)q;
      ZoftVect crnr (frnt -> Loc ()
                     +  r * cos (theeeta) * Vect::xaxis
                     +  r * sin (theeeta) * Vect::yaxis);
      RandVect jigv (jigamp, ZoftVect ());
      SumVect voitex (crnr, jigv);
      polysplat -> AppendVertex (voitex);
    }
  splat -> AppendRenderable (polysplat);
  polysplat -> SetShouldEdge (true);
  polysplat -> SetEdgeColor (ZeColor (1.0, 0.25));
  polysplat -> SetShouldFill (false);
//  g_windshield -> AppendChild (splat);
*/

/*
  TextureParticulars tipi
    = CreateTexture2D ("/tmp/SIGN.jpg", DefaultTextureFlags);
  TexturedRenderable *texre = new TexturedRenderable (tipi);
  texre -> AdjColorZoft ()
    . BecomeLike (SinuColor (ZeColor (0.0, 0.5), 1.0, ZeColor (1.0, 0.5)));
  Node *texno = (tamp.texxyno = new Node (texre));
  texno -> Scale (1300.0);
  texno -> Translate (frnt -> Loc ());
//  g_wallpaper -> AppendChild (texno);
*/

  InterpColor atmbb_wall_col (ZeColor (0.0, 0.0), ZeColor (0.5, 0.5), 0.4);
  InterpColor atmbb_tabl_col (ZeColor (0.0, 0.0), ZeColor (0.5, 0.5), 0.4);
  atmbb_wall_col . Commence ();
  atmbb_tabl_col . Commence ();
  Tamglobals::Only ()->escatom_bbox_color . BecomeLike (atmbb_wall_col);
  Tamglobals::Only ()->tabatom_bbox_color . BecomeLike (atmbb_tabl_col);

  AtomicFreezone *afz = new AtomicFreezone;
  tamp.freezo = ch_ptr <AtomicFreezone> (afz);
  afz->cineganz = &film_infos;
  afz -> SetAmokField (g_wallpaper);
  afz->underlying_maeses . push_back (frnt);
  afz->underlying_maeses . push_back (left);
  stringy_list spethial = { "c=carnosaur_01", "noth=bunny_right",
                            "pt=drunk", "pb=flashing_light", "mr=ship_01" };
  for (const std::string &s  :  spethial)
    if (! afz -> AppendPrivilegedAtom (s))
      fprintf (stderr, "phailed privileging atom [%s]...\n", s . c_str ());
  afz -> SetPrivlegedAtomProbability (0.05);
  AppendBulletinPhage (&(tamp . GetSprinkler ()), tamp.freezo);

  OeuvreAfterlife *ova = new OeuvreAfterlife;
  tamp.vreaft = ch_ptr <OeuvreAfterlife> (ova);
  Tamglobals::Only ()->valhalla = ova;
  ova -> SetAmokField (g_wallpaper);
  ova->underlying_maeses . push_back (frnt);
  ova->underlying_maeses . push_back (left);


  // g_wallpaper -> SetAdjColor (SinuZoft (ZeColor (0.0, 0.0, 0.5, 0.3), 0.16,
  //                                       ZeColor (1.0, 1.0, 0.0, 0.3)));

  LinePileRenderable *lpr = new LinePileRenderable;
  InterpColor constru_col ((SinuColor (ZeColor (0.2, 0.2), 0.25,
                                       ZeColor (0.8, 0.8))),
                           ZoftColor (ZeColor (0.0, 0.0)),
                           0.4);
  constru_col . SetInterpFunc (InterpFuncs::LINEAR);
  lpr -> SetLinesColor (constru_col);
  constru_col . Commence ();
  Tamglobals::Only ()->construction_marks_color . BecomeLike (constru_col);

  PlatonicMaes *plams[2] = { left, frnt };
  for (PlatonicMaes *emm  :  plams)
    { Vect v = 0.5 * emm->wid.val * emm->ovr.val;
      Vect l = emm->loc.val - emm->upp.val * (emm->upp.val . Dot (emm->loc.val));
      Vect l2 = l - v + Tamparams::Current ()->collband_mid * emm->upp.val;
      l += -v + Tamparams::Current ()->escaband_mid * emm->upp.val;
      Vect r = l + 2.0 * v;
      Vect r2 = l2 + 2.0 * v;
      v = 0.5 * Tamparams::Current ()->escaband_hei * emm->upp.val;
      Vect v2 = 0.5 * Tamparams::Current ()->collband_hei * emm->upp.val;
      Vect b = l - v;
      Vect t = l + v;
      Vect b2 = l2 - v2;
      Vect t2 = l2 + v2;
      afz -> AppendSwath (new Swath ({l,  r},  {b,  t},  emm));
      ova -> AppendSwath (new Swath ({l2, r2}, {b2, t2}, emm));

      lpr -> AppendLine ({l - v, l + v});
      lpr -> AppendLine ({l + v, r + v});
      lpr -> AppendLine ({r + v, r - v});
      lpr -> AppendLine ({r - v, l - v});
    }

AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes;
sherm -> SendStatus ("Running");


std::vector <std::string>  collanomer {
// "Collage01-Half-GryRnd.mp4",
// "Collage08-Half-GryRnd.mp4",    // door-bachelors
// "Collage5a-Half-GryRnd.mp4",     // turtle
// "collage06_Half-GryRndv2.mp4",  // cake-milk-tony
// "collage03-Half-GryRnd.mp4",    // bronson-dentures-mancandy
// "Collage02b_Half-GryRndv2.mp4", // zombie-vu
// "collage07b-Half-GryRnd.mp4"   // ufo
"Collage01-Half-GryRnd.mp4",
"Collage08-Blur-25.mp4",    // door-bachelors
"collage05-Blur25.mp4",     // turtle
"collage06-Blur25.mp4",  // cake-milk-tony
"collage03-Blur25.mp4",    // bronson-dentures-mancandy
"Collage02b-Blur25.mp4", // zombie-vu
"collage07b-Blur25.mp4",   // ufo
};

 for (std::string &nm  :  collanomer)
   { //std::string path = "/opt/trelopro/tamper/demo-temp/fin-col/" + nm;
     std::string path = "/opt/trelopro/tamper/demo-temp/Blurred-Ellipse/" + nm;
     ova -> AppendCollage (new Ollag (path));
   }
  ova -> DistributeCollagesEquitably ();

  Node *wframe_node = new Node (lpr);

  g_wallpaper -> AppendChild (wframe_node);
  Tamglobals::Only ()->construction_marks . push_back (wframe_node);

//  afz -> PopulateFromScratch ();
  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.freezo);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.freezo);

  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.orksu);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.orksu);
  AppendBulletinPhage (&(tamp . GetSprinkler ()), tamp.orksu);

  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.gegyp);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.gegyp);
  AppendBulletinPhage (&(tamp . GetSprinkler ()), tamp.gegyp);

  tamp . Run ();

  delete a_mess;

  return 0;
}
