#include <GraphicsApplication.hpp>

#include <Layer.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>
#include <Node.hpp>
#include <PlatonicMaes.h>
#include <Renderable.hpp>
#include <TampVideoTerminus.hpp>
#include <TexturedRenderable.hpp>
#include <VideoRenderable.hpp>
#include <ZEYowlEvent.h>

#include <BlockTimer.hpp>

#include <base_types.hpp>
#include <class_utils.hpp>
#include <bgfx_utils.hpp>

#include <algorithm>
#include <string_view>

#include <stdio.h>


using namespace charm;
namespace s2 = boost::signals2;

static ch_weak_ptr<VideoPipeline> s_pipeline;
static Node *s_renderable_owner = nullptr;
static MattedVideoRenderable *s_renderable = nullptr;
static bool s_pipeline_is_playing = true;
static FilmCatalog s_films;
static i64 s_film_index = 4;
static i64 s_clip_index = 1;
static boost::signals2::connection s_seg_done;

//increment val mod max
template<typename T>
void mod_inc_index (T &val, T max)
{
  static_assert (std::is_integral_v<T>);
  val = (val + 1) % max;
}

//decrement val mod max
template<typename T>
void mod_dec_index (T &val, T max)
{
  static_assert (std::is_integral_v<T>);
  val = (val + max - 1) % max;
}

static void AddMatteToPipeline (VideoPipeline *pipe, ClipInfo const &ci)
{
  pipe->AddMatte(ci.start_time, ci.start_time + ci.duration,
                 ci.frame_count, ci.directory,
                 ci.geometry.dir_geometry.min,
                 ci.geometry.dir_geometry.max);
}

i64 handle_key_press (s2::connection , ZEYowlAppearEvent *_event)
{
  ch_ptr<VideoPipeline> pipe = s_pipeline.ref ();
  if (! pipe)
    return 0;

  if (! pipe->GetDecoder())
    return 0;

  std::string utt = _event->Utterance();
  std::transform (utt.begin (), utt.end (), utt.begin (),
                  [] (unsigned char c) { return std::tolower (c); });

  if (utt == "q") //quit
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      s_pipeline.reset ();
      GraphicsApplication::GetApplication()->StopRunning();
    }
  else if (utt == "w" || utt == "s") //previous/next video
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      if (utt == "w")
        mod_dec_index(s_film_index, s_films.GetFilmCount ());
      else
        mod_inc_index(s_film_index, s_films.GetFilmCount ());

      s_renderable_owner->RemoveRenderable(s_renderable);

      FilmInfo const &film_info = s_films.GetNthFilm(s_film_index);

      auto *video_system = VideoSystem::GetSystem();
      VideoBrace const brace = video_system->OpenVideoFile (film_info.film_path.string());

      MattedVideoRenderable *renderable
        = new MattedVideoRenderable (brace.video_texture);
      renderable->SetEnableMatte(false);
      renderable->SetSizeReferent(SizeReferent::Video);
      s_pipeline = brace.control_pipeline;
      s_renderable_owner->AppendRenderable (renderable);
      s_renderable = renderable;
      s_pipeline_is_playing = true;
      //s_clip_index = film_info.GetClipCount() - 1;
    }
  else if (utt == "a" || utt == "d") //previous/next clip
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      FilmInfo const &fi = s_films.GetNthFilm(s_film_index);
      if (utt == "a")
        mod_dec_index(s_clip_index, fi.GetClipCount ());
      else
        mod_inc_index(s_clip_index, fi.GetClipCount ());

      pipe->ClearMattes();
      ClipInfo const &ci = fi.GetNthClip(s_clip_index);
      AddMatteToPipeline(pipe.get (), ci);
      pipe->Loop(ci.start_time, ci.start_time + ci.duration);
      s_renderable->EnableMatte();
      s_renderable->SetEnableMixColor(true);
    }
  else if (utt == "z" || utt == "c") //fast play to next clip
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      FilmInfo const &fi = s_films.GetNthFilm(s_film_index);
      if (utt == "z")
        mod_dec_index(s_clip_index, fi.GetClipCount ());
      else
        mod_inc_index(s_clip_index, fi.GetClipCount ());

      pipe->ClearMattes();
      ClipInfo const &ci = s_films.GetNthFilm(s_film_index).GetNthClip(s_clip_index);
      pipe->TrickSeekTo (ci.start_time, 1.0);
    }
  else if (utt == "e") //increase playspeed 5x
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      f32 const ps = pipe->GetDecoder()->PlaySpeed();
      pipe->SetPlaySpeed(ps + 5.0f);
    }
  else if (utt == "f") //step forward 1 frame
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      pipe->Step(1);
    }
  // else if (utt == "r") //step backward 1 frame
  //   {
  //     auto pipe = s_pipeline.ref ();
  //     pipe->Step(-1);
  //   }
  else if (utt == "p") //toggle play/pause
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      if (s_pipeline_is_playing)
        {
          fprintf (stderr, "set to paused\n");
          pipe->Pause();
        }
      else
       {
          fprintf (stderr, "set to playing\n");
          pipe->Play();
       }

      s_pipeline_is_playing = !s_pipeline_is_playing;
    }
  else if (utt == "b")
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      f64 pos = pipe->CurrentTimestamp();
      pipe->ClearMattes();
      s_renderable->DisableMatte ();
      s_renderable->SetEnableMixColor(false);
      pipe->Seek(pos);
    }
  else if (utt == "i") // loop over first three clips of itmfl
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      s_film_index = 5;
      s_clip_index = 0;
      FilmInfo const &film_info = s_films.GetNthFilm(s_film_index);

      s_renderable_owner->RemoveRenderable(s_renderable);

      auto *video_system = VideoSystem::GetSystem();
      VideoBrace const brace = video_system->OpenVideoFile (film_info.film_path.string());

      MattedVideoRenderable *renderable
        = new MattedVideoRenderable (brace.video_texture);
      renderable->SetEnableMatte(true);
      renderable->SetSizeReferent(SizeReferent::Video);
      s_pipeline = brace.control_pipeline;
      s_renderable_owner->AppendRenderable (renderable);
      s_renderable = renderable;
      s_pipeline_is_playing = true;

      for (szt i = 0; i < 3; ++i)
        {
          ClipInfo const &ci = film_info.GetNthClip(i);
          AddMatteToPipeline(pipe.get (), ci);
        }
      ClipInfo const &ci = film_info.GetNthClip(0);
      pipe->SetActiveMatte(0);
      pipe->GetDecoder()->Loop(ci.start_time, ci.start_time + ci.duration, 1.0f);
    }
  else if (utt == "j")
    {
      fprintf (stderr, "%s\n", utt.c_str ());
      i64 am = pipe->active_matte;
      mod_inc_index(am, pipe->MatteCount());
      pipe->SetActiveMatte(am);
    }

  return 0;
}

int main (int, char **)
{
  GraphicsApplication dead_zone;
  if (! dead_zone.StartUp ())
    return -1;

  Layer *layer = dead_zone.GetSceneLayer();

  Node *nodal = new Node ();
  PlatonicMaes *maes = dead_zone.FindMaesByName("front");
  nodal->Scale (0.75 * maes->Width());
  nodal->Translate (maes->Loc());

  assert (s_films.LoadFilmInfo("../configs/film-config.toml"));

  // FilmInfo &film_info = s_films[s_film_index];
  // assert (film_info.clips.size () > 0);
  // VideoRenderable *renderable = new VideoRenderable (film_info);

  assert (s_films.LoadFilmGeometry("../configs/mattes.toml"));

  FilmInfo const &film_info = s_films.GetNthFilm(s_film_index);
  assert (film_info.clips.size () > 0 && i64(film_info.clips.size ()) > s_clip_index);
  [[maybe_unused]] ClipInfo const &clip_info = film_info.GetNthClip(s_clip_index);

  auto *video_system = VideoSystem::GetSystem();
  VideoBrace brace = video_system->OpenVideoFile (film_info.film_path.string());

  MattedVideoRenderable *renderable
    = new MattedVideoRenderable (brace.video_texture);
  renderable->SetEnableMatte(false);
  renderable->SetSizeReferent(SizeReferent::Video);
  s_pipeline = brace.control_pipeline;
  // printf ("clip dims: [%u, %u] - [%u, %u]\n",
  //         clip_info.geometry.dir_geometry.min[0],
  //         clip_info.geometry.dir_geometry.min[1],
  //         clip_info.geometry.dir_geometry.max[0],
  //         clip_info.geometry.dir_geometry.max[1]);
  s_renderable = renderable;
  s_renderable_owner = nodal;
  brace = VideoBrace{};

  s2::connection key_conn = dead_zone.GetSprinkler().AppendHandler<ZEYowlAppearEvent>(&handle_key_press);

  nodal->AppendRenderable(renderable);
  layer->GetRootNode()->AppendChild(nodal);
  CMVTrefoil *tr = dead_zone.FindRenderLeafByName("front");
  tr->layers.push_back(layer);

  dead_zone.Run ();

  return 0;
}
