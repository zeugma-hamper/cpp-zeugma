#ifndef GIMME_THE_RAW_BITS_TIMELY
#define GIMME_THE_RAW_BITS_TIMELY

#include <base_types.hpp>

#include <class_utils.hpp>

#include <gst/gst.h>
#include <gst/video/video-info.h>

#include <memory>
#include <mutex>
#include <string_view>


namespace charm {

enum class MediaState
  {
    None,
    Ready,
    Paused,
    Playing,
    EOS
  };

class PipelineTerminus;

struct DecodePipeline
{
  friend PipelineTerminus;

  DecodePipeline ();
  ~DecodePipeline ();

  CHARM_DELETE_COPY(DecodePipeline);

  void PollMessages ();

  bool Open (std::string_view uri, PipelineTerminus *terminus);

  void Play ();
  void Seek (double _ts);
  void Pause ();

  void CleanUp ();

  void HandleErrorMessage (GstMessage *);
  void HandleEosMessage (GstMessage *);
  void HandleStateChangedMessage (GstMessage *);

  std::unique_ptr<PipelineTerminus> m_terminus;
  GstElement *m_pipeline;
  GstElement *m_uridecodebin;
  GstBus *m_bus;
  MediaState m_media_state;
  MediaState m_pending_state;
};

}

#endif //GIMME_THE_RAW_BITS_TIMELY
