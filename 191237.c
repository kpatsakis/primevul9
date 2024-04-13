gst_qtdemux_check_send_pending_segment (GstQTDemux * demux)
{
  if (G_UNLIKELY (demux->pending_newsegment)) {
    gint i;

    gst_qtdemux_push_pending_newsegment (demux);
    /* clear to send tags on all streams */
    for (i = 0; i < demux->n_streams; i++) {
      QtDemuxStream *stream;
      stream = demux->streams[i];
      gst_qtdemux_push_tags (demux, stream);
      if (stream->sparse) {
        GST_INFO_OBJECT (demux, "Sending gap event on stream %d", i);
        gst_pad_push_event (stream->pad,
            gst_event_new_gap (stream->segment.position, GST_CLOCK_TIME_NONE));
      }
    }
  }
}