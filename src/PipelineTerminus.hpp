#ifndef ALL_ABOARD_NIGHT_TRAIN
#define ALL_ABOARD_NIGHT_TRAIN

#include <base_types.hpp>
#include <class_utils.hpp>
#include <gst_ptr.hpp>
#include <ch_ptr.hpp>

#include <gst/gst.h>
#include <gst/video/video-frame.h>

#include <atomic>
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

  std::string GetAcceptedCapsString () const;
  virtual gst_ptr<GstCaps> GetAcceptedCaps () const = 0;
  virtual bool Accepts (GstCaps *_caps) const = 0;

  virtual bool OnStart       (DecodePipeline *) = 0;
  virtual bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *) = 0;
  virtual bool OnShutdown    (DecodePipeline *) = 0;

  f64 CurrentTimestamp () const;
  virtual gint64 CurrentTimestampNS () const;

  virtual void FlushNotify () {};

  virtual bool HasSink () const = 0;
};

enum class SampleStatus
{
  NoSample,
  Preroll,
  Play,
};

class BasicPipelineTerminus : public PipelineTerminus
{
 public:
  BasicPipelineTerminus (std::string_view _accepted_caps);
  ~BasicPipelineTerminus () override;

  gst_ptr<GstCaps> GetAcceptedCaps () const override;
  bool Accepts (GstCaps *_caps) const override;

  bool OnStart       (DecodePipeline *) override;
  bool NewDecodedPad (DecodePipeline *, GstElement *, GstPad *) override;
  bool OnShutdown    (DecodePipeline *) override;

  virtual bool HandleDecodedPad (GstElement *, GstPad *, GstCaps *) = 0;

  bool HasSink () const override;


  gst_ptr<GstCaps> m_accepted_caps;
  DecodePipeline *m_pipeline;
  GstElement *m_sink;
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
