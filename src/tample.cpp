
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
#include <OSCWandWaterWorks.hpp>
#include <ZEBulletinEvent.h>
#include <ZESpatialEvent.h>
#include <ZEYowlEvent.h>
#include <RawEventParsing.h>
#include <GeomFumble.h>

// event-adjacent(s)
#include <nlohmann/json.hpp>

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
#include "TampoChief.h"
#include "gallimaufry.h"
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


#define ERROR_RETURN_VAL(MSG, VAL)                 \
  { fprintf (stderr, "%s\n", MSG);                 \
    return VAL;                                    \
  }

#define ERROR_RETURN(MSG)                          \
  { fprintf (stderr, "%s\n", MSG);                 \
    return;                                        \
  }


Tampo::Tampo ()  :  GraphicsApplication (),
                    Zeubject (),
                    elev_trans_mult (77.0),
                    steenbeck (NULL),
                    texxyno (NULL),
                    trig_butt_simulcount (2), trig_butt_ident (8),
                    calibrating (false), elevating (false),
                    cally (nullptr)
{ // AppendSpatialPhage (&m_event_sprinkler, this);
  // AppendYowlPhage (&m_event_sprinkler, this);
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

  i64 &clac = Tamglobals::Only ()->clapper_cnt;
  if (clac  >  0)
    --clac;
  else if (clac  ==  0)
    { Tamglobals::Only ()->clapper_visuals -> AdjColorZoft ()
        . SetHard (ZeColor (1.0, 0.0));
      clac = -1;
    }

  if (bool &acty = Tamglobals::Only ()->wall_grid_active)
    { InterpColor &ic = Tamglobals::Only ()->wall_grid_fader;
      if (Tamglobals::Only ()->solo_tamp->elev_transl . Replete ())
        { ic . Set (ZeColor (1.0, 0.0));
          acty = false;
        }
    }

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


void Tampo::PressSpaceElevatorButton (const std::string floor)
{ bool &oto = Tamglobals::Only ()->room_is_scaled_oto;
  i64 fl = ((floor == "w" || floor == "first")  ?  1
            :  ((floor == "e" || floor == "second")  ?  2
                :  ((floor == "r" || floor == "third")  ?  3  :  0)));
  if (fl  <  1)
    fl = 1;
  f64 nes = (fl == 1  ?  Tamparams::Current ()->workband_elevstop
             :  (fl == 2  ?  Tamparams::Current ()->escband_elevstop
                 :  Tamparams::Current ()->collabband_elevstop));
  if (nes  !=  Tamglobals::Only ()->cur_elev_stop)
    { Tamglobals::Only ()->cur_elev_stop = -nes;
      if (oto)
        { Tamglobals::Only ()->solo_tamp->elev_transl
            . Set (Vect (0.0, -nes, 0.0));
          Tamglobals::Only ()->wall_grid_fader . Set (ZeColor (1.0, 0.75));
          Tamglobals::Only ()->wall_grid_active = true;
        }
    }
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


i64 Tampo::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (calibrating  &&  trig_partic . size () == 0)
    { // forward!
      if (cally)
        cally -> ZESpatialMove (e);
      return 0;
    }

  FlatulateCursor (e);

  recentest_pos[e -> Provenance ()] = e -> Loc ();

  if (PlatonicMaes *emm = FindMaesByName ("table"))
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

i64 Tampo::ZESpatialHarden (ZESpatialHardenEvent *e)
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

i64 Tampo::ZESpatialSoften (ZESpatialSoftenEvent *e)
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


i64 Tampo::ZEYowlAppear (ZEYowlAppearEvent *e)
{ const std::string &utt = e -> Utterance ();
  if (utt == "q")
    { InterpVect &rs = Tamglobals::Only ()->room_scaler;
      rs . Reverse ();
      rs . Commence ();
      bool &oto = Tamglobals::Only ()->room_is_scaled_oto;
      if (oto)
        elev_transl . Set (Vect::zerov);
      else
        elev_transl . Set (Vect (0.0, Tamglobals::Only ()->cur_elev_stop, 0.0));
      oto = ! oto;
    }
  else if (utt == "w"  ||  utt == "e"  ||  utt == "r")
    { PressSpaceElevatorButton (utt); }
  else if (utt  ==  "t")
    { static bool cur_vis = false;
      cur_vis = ! cur_vis;
      // for (Node *no  :  Tamglobals::Only ()->construction_marks)
      //   if (no)
      //     no -> SetVisibilityForAllLocalRenderables (cur_vis);
      Tamglobals::Only ()->construction_marks_color . Reverse ();
      Tamglobals::Only ()->construction_marks_color . Commence ();
    }
  else if (utt  ==  "c")
    { Tamglobals::Only ()->clapper_visuals -> AdjColorZoft ()
        . SetHard (ZeColor (1.0, 1.0));
      Tamglobals::Only ()->clapper_cnt
        = Tamparams::Current ()->clapper_vis_frame_cnt;
      if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
        sherm -> SendPlayBoop (3);
    }
  else if (utt == "o" || utt == "O")
    {
      std::string const default_wand = "wand-0";
      auto it = recentest_pos.find(default_wand);
      if (it == recentest_pos.end ())
        return 0;

      Vect pos = it->second;
      PlatonicMaes *maes
        = GraphicsApplication::GetApplication()->FindMaesByName("table");
      if (! maes)
        return 0;

      auto *ga = GraphicsApplication::GetApplication ();
      OSCWandWaterWorks *osc_www = nullptr;
      szt const num_ww = ga->NumWaterWorkses ();
      for (szt i = 0; i < num_ww; ++i)
        if (osc_www = dynamic_cast<OSCWandWaterWorks *> (ga->NthWaterWorks (i)); osc_www)
          break;

      if (osc_www == nullptr)
        return 0;

      Matrix44 delta;
      delta.LoadTranslation (-pos + maes->Loc ());
      fprintf (stderr, "zero calibration delta is ");
      (pos - maes->Loc ()).SpewToStderr ();
      fprintf (stderr, "\n");

      if (utt == "O")
        osc_www->theirs_to_ours_pm *= delta;
    }
  return 0;
}


extern bool json_is_not_insanely_wrapped;


namespace po = boost::program_options;


int main (int ac, char **av)
{ // well, here we go.
  std::string aud_host ("sepsis.local");
  std::string aud_port ("57121");
  std::string params_mod_filename ("");
  std::string seasonings_filename ("../configs/seasonings.toml");

 po::options_description opts_desc ("available options");
  opts_desc . add_options ()
    ("help", "hm. what do you think?")
    ("audserver-host", po::value <std::string> (&aud_host),
     "host, via ip or name, where audio server dwells")
    ("audserver-port", po::value <std::string> (&aud_port),
     "port on which audio server listens (default: 57121)")
    ("seasonings", po::value <std::string> (&seasonings_filename),
     "toml file (default: ../configs/seasonings.toml)")
    ("params-mod", po::value <std::string> (&params_mod_filename),
     "toml file with general tamparam mods")
    ("sane-json", "json slurped from liblo isn't wrapped in an array")
   ;

  po::variables_map arg_map;
  po::store (po::parse_command_line (ac, av, opts_desc), arg_map);
  po::notify (arg_map);


  if (arg_map . count ("help"))
    { std::cout << opts_desc << "\n";
      return 0;
    }

  if (arg_map . count ("sane-json"))
    json_is_not_insanely_wrapped = true;

  if (! params_mod_filename . empty ())
    Tamparams::UpdateViaTOMLFile (params_mod_filename);

  // (further to the point foregoing...)
  nlohmann::json jay_one ( nlohmann::json::object () );
  nlohmann::json jay_two { nlohmann::json::object () };
  fprintf (stderr,
           "HEAR YE: [json ( json::object () ) . is_array ()] gives %s.\n",
           jay_one . is_array ()  ?  "TRUE"  :  "FALSE");
  fprintf (stderr,
           "WHEREAS: [json { json::object () } . is_array ()] gives %s.\n",
           jay_two . is_array ()  ?  "TRUE"  :  "FALSE");

  Tampo tamp;
  Tamglobals::Only ()->solo_tamp = &tamp;
  ch_ptr <Tampo> tampogrip (&tamp);

  AppendSpatialPhage (&(tamp . GetSprinkler ()), tampogrip);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tampogrip);

  IronLung *top_lung = new IronLung;
  top_lung -> SetName ("top-level-objects");
  IronLung::AppendGlobal (top_lung);

  if (! tamp . StartUp ())
    return -1;

  Tamparams::UpdateViaTOMLFile (seasonings_filename);
fprintf(stderr,"harumph: <%s>\n", Tamparams::Current ()->asc_table_slide_audio.c_str());

  AudioMessenger *a_mess = new AudioMessenger (aud_host, aud_port);
  Tamglobals::Only ()->sono_hermes = a_mess;

  TASReceiver *audio_rejoinder_catcher = new TASReceiver ("57122");
  tamp . AppendWaterWorks (audio_rejoinder_catcher);

//  Layer *omni_layer = tamp . GetSceneLayer();
  Layer *front_layer = new Layer ();
  tamp . AppendSceneLayer (front_layer);
  Layer *left_layer = new Layer ();
  tamp . AppendSceneLayer (left_layer);
  Layer *walls_layer = new Layer ();
  tamp . AppendSceneLayer (walls_layer);
  Layer *table_layer = new Layer ();
  tamp . AppendSceneLayer (table_layer);
  Layer *omni_layer = new Layer ();
  tamp . AppendSceneLayer (omni_layer);

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

  Node *g_fiducials = new Node;
  omni_layer -> GetRootNode () -> AppendChild (g_fiducials);
  Tamglobals::Only ()->fiducials = g_fiducials;

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

  GridRenderable *gridre = new GridRenderable;
  Node *gridno = new Node (gridre);
  gridre -> SetCenter (tabl -> Loc ());
  gridre -> SetOver (tabl -> Over ());
  gridre -> SetUp (tabl -> Up ());
  gridre -> SetWidth (/*0.5 * */tabl -> Width ());
  gridre -> SetHeight (/*0.5 * */tabl -> Height ());
  gridre -> SetWarp (0.025 * tabl -> Over ());
  gridre -> SetWeft (0.025 * tabl -> Up ());
/*
gridre -> SetDiscRadius (220.0);
gridre -> SetDiscCenter (tabl -> Loc () + 150.0 * tabl -> Up ()
                         + 50.0 * tabl -> Norm ());
gridre -> SpanFractionZoft () . BecomeLike (SinuFloat (0.2, 0.4, 0.5));
*/
  gridre -> SetGridColor (ZeColor (1.0, 1.0, 1.0,  //0.075));
                                   Tamparams::Current ()->table_grid_opacity));
  g_tablecloth -> AppendChild (gridno);

  Orksur *orkp = new Orksur (*tabl);
  tamp.orksu = ch_ptr <Orksur> (orkp);
  orkp->associated_wallmaes = frnt;
  orkp -> SetTableName ("table-2");
  g_tablecloth -> AppendChild (orkp);

  tamp . GetSprinkler () . AppendPhage <TASSuggestionEvent> (tamp.orksu);
  tamp . GetSprinkler () . AppendPhage <TMPControlEvent> (tamp.orksu);

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
  grau_egyp -> JumpToFlick (0, false);
  grau_egyp -> SetBackingMaes (frnt);
  (grau_egyp -> Translate (cine_cnt)) -> SetName ("transly");
  g_front_wall -> AppendChild (grau_egyp);

  orkp -> SetAssociatedCinelib (grau_egyp);
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
  grau_chin -> JumpToFlick (1, true);
  grau_chin -> SetBackingMaes (left);
  g_left_wall -> AppendChild (grau_chin);

  tamp.gchin = ch_ptr <GraumanPalace> (grau_chin);


  InterpColor atmbb_wall_col (ZeColor (0.0, 0.0), ZeColor (0.5, 0.5), 0.4);
  InterpColor atmbb_tabl_col (ZeColor (0.0, 0.0), ZeColor (0.5, 0.5), 0.4);
  atmbb_wall_col . Commence ();
  atmbb_tabl_col . Commence ();
  Tamglobals::Only ()->escatom_bbox_color . BecomeLike (atmbb_wall_col);
  Tamglobals::Only ()->tabatom_bbox_color . BecomeLike (atmbb_tabl_col);

  AtomicFreezone *afz = new AtomicFreezone;
  tamp.freezo = ch_ptr <AtomicFreezone> (afz);
  afz->atom_count_goal = Tamparams::Current ()->escatom_wall_count;
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
  Tamglobals::Only ()->sterngerlach = afz;

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
    "Collage01-Half-GryRnd.mp4",
    "Collage08-35.mp4",    // door-bachelors
    "collage06-35.mp4",  // cake-milk-tony
    "collage05a-35.mp4",     // turtle
    "collage03-35.mp4",    // bronson-dentures-mancandy
    "Collage02b-35.mp4", // zombie-vu
    "collage07b-35.mp4",   // ufo
  };

 i64 ordnl = -5;
 for (std::string &nm  :  collanomer)
   { //std::string path = "/opt/trelopro/tamper/demo-temp/fin-col/" + nm;
     std::string path = "/opt/trelopro/tamper/demo-temp/Blurred-Ellipse/" + nm;
     Ollag *ag = new Ollag (path);
     ag->conga_directn = ordnl > 0  ?  1  :  -1;
     ag->conga_ordinal = ordnl++;
     ova -> AppendCollage (ag);
   }
  ova -> DistributeCollagesEquitably ();

  Node *wframe_node = new Node (lpr);

  g_wallpaper -> AppendChild (wframe_node);
  Tamglobals::Only ()->construction_marks . push_back (wframe_node);

  afz -> PopulateFromScratch ();

  std::vector <PlatonicMaes *> every_maes
    { frnt, left, tabl };
  Node *clavi = new Node;
  for (PlatonicMaes *ma  :  every_maes)
    if (ma)
      { PolygonRenderable *pre = new PolygonRenderable;
        Vect vv = 0.5 * (Vect::xaxis - Vect::yaxis);
        pre -> AppendVertex (ZoftVect (vv));
        vv += Vect::yaxis;
        pre -> AppendVertex (ZoftVect (vv));
        vv -= Vect::xaxis;
        pre -> AppendVertex (ZoftVect (vv));
        vv -= Vect::yaxis;
        pre -> AppendVertex (ZoftVect (vv));

        Alignifer *square = new Alignifer (pre);
        square -> AlignToMaes (ma);
        square -> ScaleZoft () . Set (Vect (0.5 * ma -> Height ()));
        square -> LocZoft () . Set (ma -> Loc ());
        clavi -> AppendChild (square);
      }
  g_fiducials -> AppendChild (clavi);
  clavi -> AdjColorZoft () . SetHard (ZeColor (1.0, 0.0));
  Tamglobals::Only ()->clapper_visuals = clavi;

  std::vector <PlatonicMaes *> wall_maeses
    { frnt, left };
  Node *walgr = new Node;
  for (PlatonicMaes *ma  :  wall_maeses)
    if (ma)
      { // GridRenderable *gridre = new GridRenderable;
        // Node *gridno = new Node (gridre);
        // gridre -> SetCenter (ma -> Loc ());
        // gridre -> SetOver (ma -> Over ());
        // gridre -> SetUp (ma -> Up ());
        // gridre -> SetWidth (ma -> Width ());
        // gridre -> SetHeight (ma -> Height ());
        // gridre -> SetWarp (0.00525 * ma -> Over ());
        // gridre -> SetWeft (0.00525 * ma -> Up ());

        // gridre -> SpanFractionZoft () . Set (0.4);

        // gridre -> SetGridColor (ZeColor (1.0, 1.0, 1.0, 0.35));
        Alignifer *gridno = PlusTimeWonderBundt (ma -> Width (),
                                                 2.0 * ma -> Height (),
                                                 550.0, 0.45);
        gridno -> AlignToMaes (ma);
        gridno -> LocZoft ()
          . Set (ma -> Loc () + 0.5 * ma -> Height () * ma -> Up ());
        walgr -> AppendChild (gridno);
      }
  g_wallpaper -> AppendChild (walgr);
  InterpColor wal_iro;
  wal_iro . SetInterpFunc (InterpFuncs::LINEAR);
  wal_iro . SetInterpTime (0.2);
  wal_iro . SetHard (ZeColor (1.0, 0.0));
  walgr -> AdjColorZoft () . BecomeLike (wal_iro);
  Tamglobals::Only ()->wall_grids = walgr;
  Tamglobals::Only ()->wall_grid_fader = wal_iro;
  Tamglobals::Only ()->wall_grid_fader . BecomeLike (wal_iro);


  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.freezo);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.freezo);

  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.orksu);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.orksu);
  AppendBulletinPhage (&(tamp . GetSprinkler ()), tamp.orksu);

  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.gegyp);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.gegyp);
  AppendBulletinPhage (&(tamp . GetSprinkler ()), tamp.gegyp);

  top_lung -> AppendBreathee (tamp.orksu . get ());
  top_lung -> AppendBreathee (tamp.gegyp . get ());
  top_lung -> AppendBreathee (tamp.gchin . get ());
  top_lung -> AppendBreathee (tamp.freezo . get ());
  top_lung -> AppendBreathee (tamp.vreaft . get ());

  tamp . Run ();

  delete a_mess;

  return 0;
}
