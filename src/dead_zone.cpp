#include <GraphicsApplication.hpp>

#include <Layer.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>
#include <Node.hpp>
#include <PlatonicMaes.h>
#include <Renderable.hpp>
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
static std::vector<FilmInfo> s_films;
static u64 s_film_index = 4;
static u64 s_clip_index = 1;
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



i64 handle_key_press (s2::connection , ZEYowlAppearEvent *_event)
{
  fprintf (stderr, "handle key press\n");
  ch_ptr<VideoPipeline> pipe = s_pipeline.ref ();
  if (! pipe)
    return 0;

  ch_ptr<DecodePipeline> dec = pipe->GetDecoder();
  if (! dec)
    return 0;

  std::string utt = _event->Utterance();
  std::transform (utt.begin (), utt.end (), utt.begin (),
                  [] (unsigned char c) { return std::tolower (c); });

  if (utt == "q") //quit
    {
      GraphicsApplication::GetApplication()->StopRunning();
    }
  else if (utt == "w" || utt == "s") //previous/next video
    {
      if (utt == "w")
        mod_dec_index(s_film_index, s_films.size ());
      else
        mod_inc_index(s_film_index, s_films.size ());

      s_renderable_owner->RemoveRenderable(s_renderable);

      FilmInfo const &film_info = s_films[s_film_index];

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
    }
  else if (utt == "a" || utt == "d") //previous/next clip
    {
      if (utt == "a")
        mod_dec_index(s_clip_index, s_films[s_film_index].clips.size ());
      else
        mod_inc_index(s_clip_index, s_films[s_film_index].clips.size ());


      auto pipe = s_pipeline.ref ();
      if (pipe)
        {
          pipe->ClearMattes();
          ClipInfo &ci = s_films[s_film_index].clips[s_clip_index];
          v2i32 min = v2i32 {i32 (ci.geometry.dir_geometry.min[0]),
            i32 (ci.geometry.dir_geometry.min[1])};
          v2i32 max = v2i32 {i32 (ci.geometry.dir_geometry.max[0]),
            i32 (ci.geometry.dir_geometry.max[1])};
          pipe->AddMatte(ci.start_time, ci.start_time + ci.duration,
                         ci.frame_count, ci.directory, min, max);
          pipe->SetActiveMatte(0);
          pipe->GetDecoder()->Loop(ci.start_time, ci.start_time + ci.duration);
          s_renderable->EnableMatte();
        }
    }
  else if (utt == "z" || utt == "c")
    {
      if (utt == "z")
        mod_dec_index(s_clip_index, s_films[s_film_index].clips.size ());
      else
        mod_inc_index(s_clip_index, s_films[s_film_index].clips.size ());


      auto pipe = s_pipeline.ref ();
      if (pipe)
        {
          pipe->ClearMattes();
          ClipInfo &ci = s_films[s_film_index].clips[s_clip_index];
          pipe->GetDecoder()->TrickModeSeek(ci.start_time, 120.0);
          // so this works pretty well, but the fast playback is (i
          // think) hiding a discontinuity. segment done message is
          // sent quite a bit before the last frame of the segment
          // reaches the pipeline sink. will probably need something
          // tighter for audio sync.
          auto seg_done_cb = [pipe, ci] (boost::signals2::connection conn,
                                         DecodePipeline *dec, SegmentDoneBehavior sdb)
          {
            v2i32 min = v2i32 {i32 (ci.geometry.dir_geometry.min[0]),
              i32 (ci.geometry.dir_geometry.min[1])};
            v2i32 max = v2i32 {i32 (ci.geometry.dir_geometry.max[0]),
              i32 (ci.geometry.dir_geometry.max[1])};
            pipe->AddMatte(ci.start_time, ci.start_time + ci.duration,
                           ci.frame_count, ci.directory, min, max);
            pipe->SetActiveMatte(0);
            pipe->GetDecoder()->Loop(ci.start_time, ci.start_time + ci.duration, 1.0f);
            s_renderable->EnableMatte();
            conn.disconnect();
          };
          s_seg_done = pipe->GetDecoder()->AddSegmentDoneExCallback(std::move (seg_done_cb));
        }
    }
  else if (utt == "e")
    {
      auto pipe = s_pipeline.ref ();
      f32 const ps = pipe->GetDecoder()->PlaySpeed();
      pipe->GetDecoder()->SetPlaySpeed(ps + 5.0f);
    }
  else if (utt == "f")
    {
      auto pipe = s_pipeline.ref ();
      pipe->GetDecoder()->Step(1);
    }
  else if (utt == "p") //toggle play/pause
    {
      if (s_pipeline_is_playing)
        {
          fprintf (stderr, "set to paused\n");
          dec->Pause();
        }
      else
       {
          fprintf (stderr, "set to playing\n");
          dec->Play();
       }

      s_pipeline_is_playing = !s_pipeline_is_playing;
    }
  else if (utt == "b")
    {
      auto pipe = s_pipeline.ref ();
      if (pipe)
        {
          f64 pos = pipe->GetDecoder()->CurrentTimestamp();
          pipe->ClearMattes();
          s_renderable->DisableMatte ();
          pipe->GetDecoder()->Seek(pos);
        }
    }
  else if (utt == "i") // loop over first three clips of itmfl
    {
      s_film_index = 5;
      s_clip_index = 0;
      FilmInfo &film_info = s_films[s_film_index];

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

      auto pipe = s_pipeline.ref ();
      if (pipe)
        {
          for (szt i = 0; i < 3; ++i)
            {
              ClipInfo &ci = film_info.clips[i];
              v2i32 min = v2i32 {i32 (ci.geometry.dir_geometry.min[0]),
                i32 (ci.geometry.dir_geometry.min[1])};
              v2i32 max = v2i32 {i32 (ci.geometry.dir_geometry.max[0]),
                i32 (ci.geometry.dir_geometry.max[1])};
              pipe->AddMatte(ci.start_time, ci.start_time + ci.duration,
                             ci.frame_count, ci.directory, min, max);
            }
          ClipInfo &ci = film_info.clips[0];
          pipe->SetActiveMatte(0);
          pipe->GetDecoder()->Loop(ci.start_time, ci.start_time + ci.duration, 1.0f);
        }
    }
  else if (utt == "j")
    {
      auto pipe = s_pipeline.ref ();
      if (pipe)
        {
          i64 am = pipe->active_matte;
          mod_inc_index(am, pipe->MatteCount());
          pipe->SetActiveMatte(am);
        }
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

  s_films = ReadFilmInfo ("../configs/film-config.toml");
  assert (s_films.size () > 0 && s_films.size () > s_film_index);

  // FilmInfo &film_info = s_films[s_film_index];
  // assert (film_info.clips.size () > 0);
  // VideoRenderable *renderable = new VideoRenderable (film_info);

  std::vector<FilmGeometry> geom = ReadFileGeometry("../configs/mattes.toml");
  assert (geom.size () > 0);
  MergeFilmInfoGeometry(s_films, geom);

  FilmInfo &film_info = s_films[s_film_index];
  assert (film_info.clips.size () > 0 && film_info.clips.size () > s_clip_index);
  [[maybe_unused]] ClipInfo &clip_info = film_info.clips[s_clip_index];

  auto *video_system = VideoSystem::GetSystem();
  VideoBrace const brace = video_system->OpenVideoFile (film_info.film_path.string());

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

  s2::connection key_conn = dead_zone.GetSprinkler().AppendHandler<ZEYowlAppearEvent>(&handle_key_press);

  nodal->AppendRenderable(renderable);
  layer->GetRootNode()->AppendChild(nodal);
  CMVTrefoil *tr = dead_zone.FindRenderLeafByName("front");
  tr->layers.push_back(layer);

  dead_zone.Run ();

  return 0;
}
