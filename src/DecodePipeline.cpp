#include "DecodePipeline.hpp"
#include "PipelineTerminus.hpp"

#include <stdio.h>

namespace charm {

DecodePipeline::DecodePipeline ()
  : m_terminus {nullptr},
    m_pipeline {nullptr},
    m_uridecodebin {nullptr},
    m_bus {nullptr},
    m_media_state {MediaState::None},
    m_pending_state {MediaState::None}
{ }

static void
db_pad_added_handler (GstElement *src, GstPad *pad, DecodePipeline *data);
static gboolean
db_autoplug_continue_handler (GstElement *bin, GstPad *pad,
                              GstCaps *caps, DecodePipeline *data);


void DecodePipeline::poll_messages ()
{
  if (! m_bus)
    return;

  GstMessage *message;
  while ((message = gst_bus_pop_filtered(m_bus, GstMessageType (GST_MESSAGE_EOS |
                                                              GST_MESSAGE_ERROR |
                                                              GST_MESSAGE_STATE_CHANGED))))
    {
      switch (GST_MESSAGE_TYPE (message)) {
      case GST_MESSAGE_EOS:
        handle_eos_message(message);
        break;
      case GST_MESSAGE_ERROR:
        handle_error_message(message);
        break;
      case GST_MESSAGE_STATE_CHANGED:
        handle_state_changed_message(message);
        break;
      default:
        fprintf (stderr, "unexpected message %d\n", GST_MESSAGE_TYPE (message));
        break;
      }

      gst_message_unref (message);
    }
}

bool DecodePipeline::open (std::string_view uri, PipelineTerminus *term)
{
  if (uri.empty() || ! term)
    return false;

  m_terminus = std::unique_ptr<PipelineTerminus> {term};
  gst_init (NULL, NULL);

  m_pipeline = gst_pipeline_new ("decode-pipeline");
  m_uridecodebin = gst_element_factory_make ("uridecodebin", "uridb");

  if (! m_pipeline || ! m_uridecodebin)
    {
      fprintf (stderr, "decode pipeline: couldn't create pipeline or decodebin\n");
      return false;
    }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
  gst_bin_add (GST_BIN (m_pipeline), m_uridecodebin);
  gst_element_sync_state_with_parent(m_uridecodebin);

  g_object_set (m_uridecodebin, "uri", uri.data (), NULL);

  g_signal_connect (m_uridecodebin, "pad-added",
                    G_CALLBACK (db_pad_added_handler), this);
  g_signal_connect (m_uridecodebin, "autoplug-continue",
                    G_CALLBACK (db_autoplug_continue_handler), this);

  return m_terminus->on_start (this);
}

void DecodePipeline::play ()
{
  if (! m_pipeline)
    return;

  gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

void DecodePipeline::seek (double _ts)
{
  if (! m_pipeline)
    return;

  const gint64 seconds_to_ns = 1000000000;

  gst_element_seek (m_pipeline, 1.0,
                    GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                    GST_SEEK_TYPE_SET, (gint64)(_ts * seconds_to_ns),
                    GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}

void DecodePipeline::pause ()
{
  if (! m_pipeline)
    return;

  gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
}

void DecodePipeline::clean_up ()
{
  if (m_terminus)
    {
      m_terminus->on_shutdown (this);
      m_terminus.reset();
    }

  if (m_bus)
    gst_object_unref (m_bus);
  m_bus = nullptr;

  if (m_pipeline)
    {
      gst_element_set_state (m_pipeline, GST_STATE_NULL);
      gst_object_unref (m_pipeline);
    }

  // not holding onto ref to m_uridecodebin
  m_uridecodebin = nullptr;
  m_pipeline = nullptr;
}

void DecodePipeline::handle_error_message (GstMessage *message)
{
  GError *error = nullptr;
  gchar *dbg_info = nullptr;
  gst_message_parse_error(message, &error, &dbg_info);

  fprintf (stderr,"ERROR from element %s: %s\n",
           GST_OBJECT_NAME (message->src), error->message);
  fprintf (stderr, "Debugging info: %s\n", (dbg_info) ? dbg_info : "none");

  g_error_free (error);
  g_free (dbg_info);
}

void DecodePipeline::handle_eos_message (GstMessage *)
{
  m_media_state = MediaState::EOS;
}

static MediaState convert_gst_state (GstState _state)
{
  if (_state == GST_STATE_PLAYING)
    return MediaState::Playing;
  else if (_state == GST_STATE_PAUSED)
    return MediaState::Paused;
  else if (_state == GST_STATE_READY)
    return MediaState::Ready;
  else if (_state == GST_STATE_NULL)
    return MediaState::None;

  return MediaState::None;
}

void DecodePipeline::handle_state_changed_message (GstMessage *message)
{
  GstState old_state, new_state, pend_state;
  gst_message_parse_state_changed(message, &old_state, &new_state, &pend_state);

  m_media_state = convert_gst_state (new_state);
  m_pending_state = convert_gst_state (pend_state);
}



DecodePipeline::~DecodePipeline ()
{
  clean_up ();
}

/* This function will be called by the pad-added signal */
static void db_pad_added_handler (GstElement *src, GstPad *new_pad, DecodePipeline *data)
{
  data->m_terminus->new_decoded_pad(data, src, new_pad);
}

static gboolean db_autoplug_continue_handler (GstElement *, GstPad *,
                                              GstCaps *, DecodePipeline *)
{
  // TODO: here later select the hardware decoder
  // GstStructure *structure = gst_caps_get_structure (caps, 0);
  // gchar const *struct_name = gst_structure_get_name (structure);

  return TRUE;
}

}
