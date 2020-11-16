
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
#include <PolygonRenderable.h>

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

//events
#include <GLFWWaterWorks.hpp>
#include <ZESpatialEvent.h>
#include <ZEYowlEvent.h>
#include <RawEventParsing.h>
#include <GeomFumble.h>

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


using namespace charm;

// don't forget: oy.
bool extra_poo = [] () { srand48 (32123);  return true; } ();


class Cursoresque  :  public Alignifer
{ public:
  PolygonRenderable *re1,  *re2;
  Cursoresque (f64 sz, i64 nv = 6)  :  Alignifer (),
                                       re1 (new PolygonRenderable),
                                       re2 (new PolygonRenderable)
    { AppendRenderable (re1);
      AppendRenderable (re2);
      re1 -> SetFillColor (ZeColor (1.0, 0.5));
      re2 -> SetFillColor (ZeColor (1.0, 0.5));
      for (i64 w = 0  ;  w < 2  ;  ++w)
        for (i64 q = 0  ;  q < nv  ;  ++q)
          { f64 theeeta = 2.0 * M_PI / (f64)nv * (f64)q  +  w * M_PI;
            Vect radial = (0.5 * (w + 1.0))
              *  (cos (theeeta) * Vect::xaxis  +  sin (theeeta) * Vect::yaxis);
            SinuVect arm (0.065 * radial, 5.0 + 0.7 * drand48 (),
                          0.24 * (1.0 + 3.0 * (q%2)) * radial);
            (w > 0 ? re1 : re2) -> AppendVertex (arm);
          }
      ScaleZoft () = Vect (sz);
    }
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

  PlatonicMaes *ClosestIntersectedMaes (const Vect &frm, const Vect &aim,
                                        Vect *hit_point = NULL);
  Frontier *IntersectedFrontier (const Vect &frm, const Vect &aim,
                                 Vect *hit_point = NULL);
  void FlatulateCursor (ZESpatialMoveEvent *e);

  void AccrueElevatorOffset (const Vect &off);

 protected:
  ch_ptr<Sensorium> sensy;

 public:
//  ch_ptr <Orksur> ork;
  ZoftVect elev_transl;
  f64 elev_trans_mult;
  VideoRenderable *steenbeck;
  ch_ptr <AtomicFreezone> freezo;
  Node *texxyno;
};


static Tampo *solo_tamp = NULL;


struct QuadroPod
{ Node *no;
  PlatonicMaes *ma;
  Renderable *re;
  Vect off;
};

std::unordered_map <std::string, QuadroPod> rupaul_map;


i64 Sensorium::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (calibrating  &&  trig_partic . size () == 0)
    { // forward!
      if (cally)
        cally -> ZESpatialMove (e);
      return 0;
    }

  Tampo *tam = (Tampo *)GraphicsApplication::GetApplication();

  if (tam)
    tam -> FlatulateCursor (e);

  recentest_pos[e -> Provenance ()] = e -> Loc ();
  if (elevating)
    { Vect newpos = AveragePos ();
      Vect offset = newpos - elev_prevpos;
      offset = offset . Dot (Vect::yaxis) * Vect::yaxis;
      if (tam)
        tam -> AccrueElevatorOffset (offset);
      elev_prevpos = newpos;
    }
  else
    {
/*    auto it = rupaul_map . find (e -> Provenance ());
      if (it != rupaul_map . end ())
        { QuadroPod &qu = it->second;
          Vect hit;
          Tampo *app = (Tampo *)GraphicsApplication::GetApplication();
          if (PlatonicMaes *maes
              = app-> ClosestIntersectedMaes (e -> Loc (), e -> Aim (), &hit))
            { if (qu.ma  !=  maes)
                { qu.ma = maes;
                  for (Renderable *rendy  :  qu.no -> GetRenderables ())
                    { rendy -> SetOver (maes -> Over ());
                      rendy -> SetUp (maes -> Up ());
                    }
                }
              if (CineAtom *ca = dynamic_cast <CineAtom *> (qu.no))
                { if (qu.re)
                    { hit -= (qu.off.x * qu.re -> Over ()
                              +  qu.off.y * qu.re -> Up ());
                      ca->loc = hit;
                    }
                }
            }
        } */
    }

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
/*  else if (Frontier *f = instance -> IntersectedFrontier
           (e -> Loc (), e -> Aim (), &hit))
    { Vect offs;
      Renderable *r = NULL;
      if (CineAtom *ca = dynamic_cast <CineAtom *> (f -> ItsNode ()))
        if (RectRenderableFrontier *rrf
            = dynamic_cast <RectRenderableFrontier *> (f))
          if (r = rrf -> GetRenderable ())
            { Vect del = hit - ca->loc;
              offs
                . Set (del . Dot (r -> Over ()), del . Dot (r -> Up ()), 0.0);
            }
      rupaul_map[e -> Provenance ()] = { f -> ItsNode (), NULL, r, offs };
    }
*/
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

  auto it = elev_partic . find (e -> Provenance ());
  if (it  !=  elev_partic . end ())
    { elev_partic . erase (it);
      if (elevating)
        {  elevating = false; }
    }
/*  else
    { auto it = rupaul_map . find (e -> Provenance ());
      if (it != rupaul_map . end ())
        rupaul_map . erase (it);
    }
*/
  return 0;
}


i64 Sensorium::ZEYowlAppear (ZEYowlAppearEvent *e)
{ if (! solo_tamp->steenbeck  ||  ! e)
    return -1;
  const ch_ptr <DecodePipeline> deep
    = solo_tamp->steenbeck -> GetPipeline ();

  if (! deep)
    return 0;

  static bool now_playing = true;
  auto seek_ts = [] () { static i32 i = 0; return ++i * 60.0; };

  fprintf (stderr, "timestamp is %.3f\n", deep -> CurrentTimestamp ());

  if (e -> Utterance ()  ==  " ")
    { if (now_playing)
        deep -> Pause ();
      else
        deep -> Play ();
      now_playing = ! now_playing;
    }
  else if (e -> Utterance ()  ==  "s")
    deep -> Seek (seek_ts ());

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
//                    ork (NULL),
                    elev_trans_mult (77.0),
                    steenbeck (NULL),
                    texxyno (NULL)
{ AppendSpatialPhage (&m_event_sprinkler, sensy);
  AppendYowlPhage (&m_event_sprinkler, sensy);
  elev_transl . MakeBecomeLikable ();
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

  if (texxyno  &&  timey.val > 1.5)
    { if (Node *par = texxyno -> Parent ())
        { par -> ExciseChild (texxyno);
          delete texxyno;
          texxyno = NULL;
        }
    }

  if (freezo)//  &&  timey.val > 3.0)
    freezo -> Inhale (ratch, thyme);
  return true;
}

PlatonicMaes *Tampo::ClosestIntersectedMaes (const Vect &frm, const Vect &aim,
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
      crs ->AlignToMaes (emm);
    }
}


void Tampo::AccrueElevatorOffset (const Vect &off)
{ Vect hither = elev_transl.val + elev_trans_mult * off;
  elev_transl = hither;
}


namespace po = boost::program_options;


int main (int ac, char **av)
{ po::options_description desc ("available options");
  desc . add_options ()
    ("help", "hm. what do you think?")
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
  po::store (po::parse_command_line(ac, av, desc), arg_map);
  po::notify(arg_map);


  if (arg_map . count ("help"))
    { std::cout << desc << "\n";
      return 0;
    }
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

  Layer *front_layer = tamp . GetSceneLayer();
  Layer *left_layer = new Layer ();
  tamp . AppendSceneLayer (left_layer);
  Layer *omni_layer = new Layer ();
  tamp . AppendSceneLayer (omni_layer);

  i32 const leaf_count = tamp . NumRenderLeaves();
  for (i32 i = 0; i < leaf_count; ++i)
    { CMVTrefoil *leaf = tamp . NthRenderLeaf(i);
      if (! leaf->maes)
        continue;

      if (leaf->maes -> Name () == "front")
        leaf->layers . push_back (front_layer);
      else if (leaf->maes -> Name () == "left")
        leaf->layers . push_back (left_layer);

      //ee_layer goes on all
      leaf->layers . push_back (omni_layer);
    }

  std::vector <FilmInfo> film_infos
    = ReadFilmInfo ("../configs/film-config.toml");
  assert (film_infos.size () > 0);

  { BlockTimer bt ("loading & merging geom");
    BlockTimer slurpt ("slurping geom file");
    std::vector<FilmGeometry> geoms
      = ReadFileGeometry("../configs/mattes.toml");
    slurpt . StopTimer ();
    assert (geoms . size ()  >  0);
    MergeFilmInfoGeometry (film_infos, geoms);
  }

  PlatonicMaes *maes = tamp . FindMaesByName ("front");
  assert (maes);
  PlatonicMaes *left = tamp . FindMaesByName ("left");
  assert (left);
  PlatonicMaes *tabl = tamp . FindMaesByName ("table");
  assert (tabl);

  f64 const total_height = 2.0 * maes -> Height();
  f64 const total_width = maes -> Width ();
  f64 const band_height = total_height / 3.0;

  Node *kawntent = new Node;
  kawntent -> Translate (tamp . elev_transl);
  omni_layer -> GetRootNode () -> AppendChild (kawntent);

  Node *windshield = new Node;
  omni_layer -> GetRootNode () -> AppendChild (windshield);

  Vect left_cntr
    = (-0.5 * (maes -> Width () - left -> Width ()) * left -> Over ()
       -  0.5 * maes -> Width () * maes -> Over ()
       +  maes -> Loc () . Dot (maes -> Up ()) * maes -> Up ());
  // elements_band = new ElementsBand (total_width, band_height, film_infos,
  //                                   *left, left_cntr);
  // elements_band->Translate (tamp . elev_transl);

  // ee_layer -> GetRootNode () -> AppendChild (elements_band);

  for (int q = 0  ;  q < 3  ;  ++q)
    { Cursoresque *c = new Cursoresque (0.015 * maes -> Height ());
      omni_layer -> GetRootNode () -> AppendChild (c);

      cursoresques . push_back (c);
      c -> LocZoft () = maes -> Loc ();
    }
/*
  tamp.steenbeck = new VideoRenderable (film_infos[4]);
  Node *drive_in = new Node (tamp.steenbeck);
  drive_in -> Scale (0.7 * total_width);
  drive_in -> Translate (maes -> Loc ()
                         - 0.9 * maes -> Height () * maes -> Up ());
  kawntent -> AppendChild (drive_in);
*/

/*
  Node *splat = new Node;
  PolygonRenderable *polysplat = new PolygonRenderable;
  i64 nv = 14;
  SinuVect centz (1000.0 * maes -> Over (), 0.3, maes -> Loc ());
  for (i64 q = 0  ;  q < nv  ;  ++q)
    { f64 r = 0.3 * maes -> Height ();
      f64 theeeta = 2.0 * M_PI / (f64)nv * (f64)q;
      Vect radv = cos (theeeta) * Vect::xaxis  +  sin (theeeta) * Vect::yaxis;
      SinuVect arm (0.3 * r * radv, 1.0 + 0.3 * drand48 (),
                    0.5 * (1.0 + q%2) * r * radv);
      SumVect voit (arm, centz);
      polysplat -> AppendVertex (voit);
    }
  polysplat -> SetFillColor (SinuZoft (ZeColor (0.0, 0.0, 0.5, 0.3), 6.0,
                                       ZeColor (1.0, 1.0, 0.0, 0.3)));
  splat -> AppendRenderable (polysplat);
  polysplat -> SetShouldEdge (true);
//  windshield -> AppendChild (splat);

  ch_ptr <Orksur> orkp (new Orksur (*tabl));
  windshield -> AppendChild (orkp . get ());
  AppendSpatialPhage (&(tamp . GetSprinkler ()), orkp);
  AppendYowlPhage (&(tamp . GetSprinkler ()), orkp);
*/
  for (i64 q = 0  ;  q < tamp . NumWaterWorkses ()  ;  ++q)
    if (GLFWWaterWorks *ww
        = dynamic_cast <GLFWWaterWorks *> (tamp . NthWaterWorks (q)))
      ww -> SetPromoteMouseToSpatialOrthoStyle (true);
/*
  for (int q = 0  ;  q < 25  ;  ++q)
    { Ticato *tic = new Ticato (film_infos);
      tic->re -> SetOver (maes -> Over ());
      tic->re -> SetUp (maes -> Up ());
      tic->sca . Set (Vect (200.0));
      tic->loc . Set (maes -> Loc ()
                      + (drand48 () - 0.5) * 6000.0 * maes -> Over ()
                      + (drand48 () - 0.5) * 6000.0 * maes -> Up ());
      windshield -> AppendChild (tic->no);
    }
*/
  TextureParticulars tipi = CreateTexture2D ("/tmp/blap.png", DefaultTextureFlags);
  TexturedRenderable *texre = new TexturedRenderable (tipi);
  Node *texno = (tamp.texxyno = new Node (texre));
  texno -> Scale (1300.0);
  texno -> Translate (maes -> Loc ());
  windshield -> AppendChild (texno);

  AtomicFreezone *afz = new AtomicFreezone;
  tamp.freezo = ch_ptr <AtomicFreezone> (afz);
  afz->cineganz = &film_infos;
  afz->field_amok = kawntent;
  afz->atom_count_goal = 45.0;
  afz->inter_arrival_t = 5.0;
  afz->min_speed = 50.0;
  afz->max_speed = 250.0;

  PlatonicMaes *plams[2] = { left, maes };
  for (PlatonicMaes *emm  :  plams)
    { Vect v = 0.5 * emm->wid.val * emm->ovr.val;
      Vect l = emm->loc.val - v;
      Vect r = l + 2.0 * v;
      v = 0.1 * emm->hei.val * emm->upp.val;
      Vect b = l - v;
      Vect t = l + v;
      afz -> AppendSwath (new Swath ({l, r}, {b, t}, emm));
    }

//  afz -> PopulateFromScratch ();
  AppendSpatialPhage (&(tamp . GetSprinkler ()), tamp.freezo);
  AppendYowlPhage (&(tamp . GetSprinkler ()), tamp.freezo);

  tamp . Run ();

  return 0;
}
