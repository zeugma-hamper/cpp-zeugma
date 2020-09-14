#ifndef ALL_ABOARD_NIGHT_TRAIN
#define ALL_ABOARD_NIGHT_TRAIN

#include "class_utils.hpp"
#include "gst_ptr.hpp"

#include <gst/gst.h>

#include <mutex>

namespace charm {

class DecodePipeline;

class PipelineTerminus
{
 public:

  PipelineTerminus ();
  virtual ~PipelineTerminus ();

  CHARM_DELETE_MOVE_COPY(PipelineTerminus);

  virtual bool OnStart       (DecodePipeline *) = 0;
  virtual bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *) = 0;
  virtual bool OnShutdown    (DecodePipeline *) = 0;
};


//Really, this only offers the ability to turn off or on audio.
class BasicPipelineTerminus : public PipelineTerminus
{
 public:
  BasicPipelineTerminus (bool _enable_audio);
  ~BasicPipelineTerminus () override;

  bool OnStart       (DecodePipeline *) override;
  bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *) override;
  bool OnShutdown    (DecodePipeline *) override;

  gst_ptr<GstSample> FetchSample ();
  gst_ptr<GstSample> FetchClearSample ();


  bool HandleAudioPad (DecodePipeline *, GstElement *, GstPad *, const char *);
  bool HandleVideoPad (DecodePipeline *, GstElement *, GstPad *, const char *);

  void HandoffSample (GstSample *_sample);

  GstElement *m_video_sink;
  GstElement *m_audio_sink;
  std::mutex m_sample_mutex;
  gst_ptr<GstSample> m_sample;
  bool m_enable_audio;
};


}

#endif //ALL_ABOARD_NIGHT_TRAIN
