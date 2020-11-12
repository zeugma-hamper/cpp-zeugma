#ifndef GIMME_THE_RAW_BITS_TIMELY
#define GIMME_THE_RAW_BITS_TIMELY

#include <base_types.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <gst/gst.h>
#include <gst/video/video-info.h>

#include <memory>
#include <mutex>
#include <string_view>


namespace charm {

class PipelineTerminus;

struct LoopStatus
{
  gint64 loop_start = -1;
  gint64 loop_end = -1;
};

struct QueuedSeek
{
  f64 rate;
  GstFormat format;
  GstSeekFlags flags;
  GstSeekType start_type;
  i64 start;
  GstSeekType stop_type;
  i64 stop;
};

struct DecodePipeline : public CharmBase<DecodePipeline>
{
  friend PipelineTerminus;

  DecodePipeline ();
  ~DecodePipeline ();

  CHARM_DELETE_COPY(DecodePipeline);

  void PollMessages ();

  bool OpenVideoFile (std::string_view _uri, PipelineTerminus *_terminus);
  bool OpenMatteSequence (std::string_view _pattern, PipelineTerminus *_terminus);

  void Play ();
  void Seek (f64 _ts);
  void Pause ();

  void Step (u32 _distance);

  void Loop (f64 _from, f64 _to);

  void SetState (GstState _state);

  f64 Duration () const;
  gint64 DurationNanoseconds () const;

  bool SeekFull (f64 rate, GstFormat format, GstSeekFlags flags,
                 GstSeekType start_type, i64 start,
                 GstSeekType stop_type, i64 stop);

  void CleanUp ();

  void HandleErrorMessage (GstMessage *);
  void HandleEosMessage (GstMessage *);
  void HandleStateChangedMessage (GstMessage *);
  void HandleSegmentDone (GstMessage *);
  void HandleAsyncDone (GstMessage *);
  void HandleDuration (GstMessage *);

  std::unique_ptr<PipelineTerminus> m_terminus;
  GstElement *m_pipeline;
  GstElement *m_uridecodebin;
  GstBus *m_bus;

  GstState m_media_state;
  GstState m_pending_state;

  f32 m_play_speed;
  gint64 m_duration;
  bool m_awaiting_async_done;
  bool m_has_eos;
  bool m_has_queued_seek;
  LoopStatus m_loop_status;
  QueuedSeek m_queued_seek;
};

}

#endif //GIMME_THE_RAW_BITS_TIMELY
