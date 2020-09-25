#ifndef BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH
#define BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>
#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

namespace charm
{

class VideoSystem
{
 public:

  CHARM_DELETE_MOVE_COPY(VideoSystem);

  static bool Initialize ();
  static void ShutDown ();

  static VideoSystem *GetSystem ();

  

 protected:
  VideoSystem ();
  ~VideoSystem ();
};

}

#endif  //BUFFERS_SAMPLES_WIKY_WIKY_WAAAAHHHHH
