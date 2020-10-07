#ifndef ALL_ABOARD_NIGHT_TRAIN
#define ALL_ABOARD_NIGHT_TRAIN

#include <base_types.hpp>
#include <class_utils.hpp>
#include <gst_ptr.hpp>

#include <gst/gst.h>
#include <gst/video/video-frame.h>

#include <mutex>
#include <vector>

namespace charm {

class DecodePipeline;

class PipelineTerminus
{
 public:

  PipelineTerminus ();
  virtual ~PipelineTerminus ();

  CHARM_DELETE_MOVE_COPY(PipelineTerminus);

  virtual bool OnStart       (DecodePipeline *) = 0;
  virtual bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *, GstCaps *) = 0;
  virtual bool OnShutdown    (DecodePipeline *) = 0;

  virtual void FlushNotify () = 0;

  virtual bool HasSink () const = 0;
};


//Really, this only offers the ability to turn off or on audio.
class BasicPipelineTerminus : public PipelineTerminus
{
 public:
  BasicPipelineTerminus (bool _enable_audio);
  ~BasicPipelineTerminus () override;

  bool OnStart       (DecodePipeline *) override;
  bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *, GstCaps *) override;
  bool OnShutdown    (DecodePipeline *) override;

  void FlushNotify () override;

  bool HasSink () const override;

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

class CachingPipelineTerminus : public PipelineTerminus
{
 public:
  CachingPipelineTerminus (bool _enable_audio, u32 _max_cache_samples);
  ~CachingPipelineTerminus () override;

  bool OnStart       (DecodePipeline *) override;
  bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *, GstCaps *) override;
  bool OnShutdown    (DecodePipeline *) override;

  void FlushNotify () override;

  bool HasSink () const override;

  gst_ptr<GstSample> FetchSample ();
  gst_ptr<GstSample> FetchClearSample ();
  gst_ptr<GstSample> FetchByOffset (u64 _offset, u64 _mod);
  gst_ptr<GstSample> FetchClearByOffset (u64 _offset, u64 _mod);

  gst_ptr<GstSample> FetchNewest ();
  void ClearCache ();

  bool HandleAudioPad (DecodePipeline *, GstElement *, GstPad *, const char *);
  bool HandleVideoPad (DecodePipeline *, GstElement *, GstPad *, const char *);

  void HandoffSample (GstSample *_sample);

  DecodePipeline *m_pipeline;
  GstElement *m_video_sink;
  GstElement *m_audio_sink;
  std::mutex m_sample_mutex;
  u32 m_max_cached_samples;
  std::vector<gst_ptr<GstSample>> m_samples;
  bool m_enable_audio;
};

//helper struct to hold onto GstSample's ref until after upload
struct video_frame_holder
{
  gst_ptr<GstSample> sample;
  GstVideoFrame video_frame;

  CHARM_DELETE_MOVE_COPY(video_frame_holder);

  video_frame_holder (gst_ptr<GstSample> const &_sample, GstVideoInfo *_info)
    : sample {_sample}
  {
    GstBuffer *buffer = gst_sample_get_buffer(_sample.get ());
    gst_video_frame_map (&video_frame, _info, buffer, GST_MAP_READ);
  }

  ~video_frame_holder ()
  {
    gst_video_frame_unmap(&video_frame);
  }
};


}

#endif //ALL_ABOARD_NIGHT_TRAIN
