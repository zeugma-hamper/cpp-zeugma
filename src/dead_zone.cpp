#include <GraphicsApplication.hpp>

#include <Layer.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>
#include <Node.hpp>
#include <PlatonicMaes.h>
#include <Renderable.hpp>
#include <VideoRenderable.hpp>
#include <ZEYowlEvent.h>

#include <BlockTimer.hpp>

#include <base_types.hpp>
#include <class_utils.hpp>
#include <bgfx_utils.hpp>

#include <string_view>

#include <stdio.h>


using namespace charm;
namespace s2 = boost::signals2;

static ch_weak_ptr<DecodePipeline> s_pipeline;
static bool s_pipeline_is_playing = true;

i64 handle_key_press (s2::connection , ZEYowlAppearEvent *_event)
{
  fprintf (stderr, "handle key press\n");
  if (auto pipe = s_pipeline.lock();
      pipe && _event->Utterance() == "p")
    {
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (pipe->m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "decode-pipeline-before");

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
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (pipe->m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "decode-pipeline-after");
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

  std::vector<FilmInfo> configs = ReadFilmInfo ("../configs/film-config.toml");
  assert (configs.size () > 0);

  FilmInfo &film_info = configs[4];
  assert (film_info.clips.size () > 0);
  VideoRenderable *renderable = new VideoRenderable (film_info);

  // std::vector<FilmGeometry> geom = ReadFileGeometry("../configs/mattes.toml");
  // assert (geom.size () > 0);
  // MergeFilmInfoGeometry(configs, geom);

  // FilmInfo &film_info = configs[4];
  // assert (film_info.clips.size () > 0);
  // [[maybe_unused]] ClipInfo &clip_info = film_info.clips[0];

  // MattedVideoRenderable *renderable
  //   = new MattedVideoRenderable (film_info, clip_info);
  // printf ("clip dims: [%u, %u] - [%u, %u]\n",
  //         clip_info.geometry.dir_geometry.min[0],
  //         clip_info.geometry.dir_geometry.min[1],
  //         clip_info.geometry.dir_geometry.max[0],
  //         clip_info.geometry.dir_geometry.max[1]);

  s_pipeline = renderable->GetPipeline();
  // s_pipeline.lock ()->Play ();

  s2::connection key_conn = dead_zone.GetSprinkler().AppendHandler<ZEYowlAppearEvent>(&handle_key_press);

  nodal->AppendRenderable(renderable);
  layer->GetRootNode()->AppendChild(nodal);
  CMVTrefoil *tr = dead_zone.FindRenderLeafByName("front");
  tr->layers.push_back(layer);

  dead_zone.Run ();

  return 0;
}
