gst_qtdemux_chain (GstPad * sinkpad, GstObject * parent, GstBuffer * inbuf)
{
  GstQTDemux *demux;

  demux = GST_QTDEMUX (parent);

  GST_DEBUG_OBJECT (demux,
      "Received buffer pts:%" GST_TIME_FORMAT " dts:%" GST_TIME_FORMAT
      " offset:%" G_GUINT64_FORMAT " size:%" G_GSIZE_FORMAT " demux offset:%"
      G_GUINT64_FORMAT, GST_TIME_ARGS (GST_BUFFER_PTS (inbuf)),
      GST_TIME_ARGS (GST_BUFFER_DTS (inbuf)), GST_BUFFER_OFFSET (inbuf),
      gst_buffer_get_size (inbuf), demux->offset);

  if (GST_BUFFER_FLAG_IS_SET (inbuf, GST_BUFFER_FLAG_DISCONT)) {
    gboolean is_gap_input = FALSE;
    gint i;

    GST_DEBUG_OBJECT (demux, "Got DISCONT, marking all streams as DISCONT");

    for (i = 0; i < demux->n_streams; i++) {
      demux->streams[i]->discont = TRUE;
    }

    /* Check if we can land back on our feet in the case where upstream is
     * handling the seeking/pushing of samples with gaps in between (like
     * in the case of trick-mode DASH for example) */
    if (demux->upstream_format_is_time
        && GST_BUFFER_OFFSET (inbuf) != GST_BUFFER_OFFSET_NONE) {
      gint i;
      for (i = 0; i < demux->n_streams; i++) {
        guint32 res;
        GST_LOG_OBJECT (demux,
            "Stream #%d , checking if offset %" G_GUINT64_FORMAT
            " is a sample start", i, GST_BUFFER_OFFSET (inbuf));
        res =
            gst_qtdemux_find_index_for_given_media_offset_linear (demux,
            demux->streams[i], GST_BUFFER_OFFSET (inbuf));
        if (res != -1) {
          QtDemuxSample *sample = &demux->streams[i]->samples[res];
          GST_LOG_OBJECT (demux,
              "Checking if sample %d from stream %d is valid (offset:%"
              G_GUINT64_FORMAT " size:%" G_GUINT32_FORMAT ")", res, i,
              sample->offset, sample->size);
          if (sample->offset == GST_BUFFER_OFFSET (inbuf)) {
            GST_LOG_OBJECT (demux,
                "new buffer corresponds to a valid sample : %" G_GUINT32_FORMAT,
                res);
            is_gap_input = TRUE;
            /* We can go back to standard playback mode */
            demux->state = QTDEMUX_STATE_MOVIE;
            /* Remember which sample this stream is at */
            demux->streams[i]->sample_index = res;
            /* Finally update all push-based values to the expected values */
            demux->neededbytes = demux->streams[i]->samples[res].size;
            demux->todrop = 0;
            demux->offset = GST_BUFFER_OFFSET (inbuf);
          }
        }
      }
      if (!is_gap_input) {
        /* Reset state if it's a real discont */
        demux->neededbytes = 16;
        demux->state = QTDEMUX_STATE_INITIAL;
        demux->offset = GST_BUFFER_OFFSET (inbuf);
      }
    }
    /* Reverse fragmented playback, need to flush all we have before
     * consuming a new fragment.
     * The samples array have the timestamps calculated by accumulating the
     * durations but this won't work for reverse playback of fragments as
     * the timestamps of a subsequent fragment should be smaller than the
     * previously received one. */
    if (!is_gap_input && demux->fragmented && demux->segment.rate < 0) {
      gst_qtdemux_process_adapter (demux, TRUE);
      for (i = 0; i < demux->n_streams; i++)
        gst_qtdemux_stream_flush_samples_data (demux, demux->streams[i]);
    }
  }

  gst_adapter_push (demux->adapter, inbuf);

  GST_DEBUG_OBJECT (demux,
      "pushing in inbuf %p, neededbytes:%u, available:%" G_GSIZE_FORMAT, inbuf,
      demux->neededbytes, gst_adapter_available (demux->adapter));

  return gst_qtdemux_process_adapter (demux, FALSE);
}