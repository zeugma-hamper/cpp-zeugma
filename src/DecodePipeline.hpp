#ifndef GIMME_THE_RAW_BITS_TIMELY
#define GIMME_THE_RAW_BITS_TIMELY

#include <base_types.hpp>

#include <utils.hpp>

#include <gst/gst.h>
#include <gst/video/video-info.h>

#include <memory>
#include <mutex>
#include <string_view>


namespace literal {

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

  void poll_messages ();

  bool open (std::string_view uri, PipelineTerminus *terminus);

  void play ();
  void seek (double _ts);
  void pause ();

  void clean_up ();

  void handle_error_message (GstMessage *);
  void handle_eos_message (GstMessage *);
  void handle_state_changed_message (GstMessage *);

  std::unique_ptr<PipelineTerminus> m_terminus;
  GstElement *m_pipeline;
  GstElement *m_uridecodebin;
  GstBus *m_bus;
  MediaState m_media_state;
  MediaState m_pending_state;
};

}

#endif //GIMME_THE_RAW_BITS_TIMELY
