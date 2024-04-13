gst_qtdemux_push_event (GstQTDemux * qtdemux, GstEvent * event)
{
  guint n;
  gboolean has_valid_stream = FALSE;
  GstEventType etype = GST_EVENT_TYPE (event);

  GST_DEBUG_OBJECT (qtdemux, "pushing %s event on all source pads",
      GST_EVENT_TYPE_NAME (event));

  for (n = 0; n < qtdemux->n_streams; n++) {
    GstPad *pad;
    QtDemuxStream *stream = qtdemux->streams[n];
    GST_DEBUG_OBJECT (qtdemux, "pushing on pad %i", n);

    if ((pad = stream->pad)) {
      has_valid_stream = TRUE;

      if (etype == GST_EVENT_EOS) {
        /* let's not send twice */
        if (stream->sent_eos)
          continue;
        stream->sent_eos = TRUE;
      }

      gst_pad_push_event (pad, gst_event_ref (event));
    }
  }

  gst_event_unref (event);

  /* if it is EOS and there are no pads, post an error */
  if (!has_valid_stream && etype == GST_EVENT_EOS) {
    gst_qtdemux_post_no_playable_stream_error (qtdemux);
  }
}