#ifndef ALL_ABOARD_NIGHT_TRAIN
#define ALL_ABOARD_NIGHT_TRAIN

#include "utils.hpp"
#include "gst_ptr.hpp"

#include <gst/gst.h>

#include <mutex>

namespace literal {

class DecodePipeline;

class PipelineTerminus
{
 public:

  PipelineTerminus ();
  virtual ~PipelineTerminus ();

  CHARM_DELETE_MOVE_COPY(PipelineTerminus);

  virtual bool on_start        (DecodePipeline *) = 0;
  virtual bool new_decoded_pad (DecodePipeline *, GstElement *, GstPad *) = 0;
  virtual bool on_shutdown     (DecodePipeline *) = 0;
};


//Really, this only offers the ability to turn off or on audio.
class BasicPipelineTerminus : public PipelineTerminus
{
 public:
  BasicPipelineTerminus (bool _enable_audio);
  ~BasicPipelineTerminus () override;

  bool on_start        (DecodePipeline *) override;
  bool new_decoded_pad (DecodePipeline *, GstElement *, GstPad *) override;
  bool on_shutdown     (DecodePipeline *) override;

  gst_ptr<GstSample> fetch_sample ();

  bool handle_audio_pad (DecodePipeline *, GstElement *, GstPad *, const char *);
  bool handle_video_pad (DecodePipeline *, GstElement *, GstPad *, const char *);

  void handoff_sample (GstSample *_sample);

  GstElement *m_video_sink;
  GstElement *m_audio_sink;
  std::mutex m_sample_mutex;
  gst_ptr<GstSample> m_sample;
  bool m_enable_audio;
};


}

#endif //ALL_ABOARD_NIGHT_TRAIN
