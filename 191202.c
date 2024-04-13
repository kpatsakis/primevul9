qtdemux_prepare_streams (GstQTDemux * qtdemux)
{
  gint i;
  GstFlowReturn ret = GST_FLOW_OK;

  GST_DEBUG_OBJECT (qtdemux, "prepare streams");

  for (i = 0; ret == GST_FLOW_OK && i < qtdemux->n_streams; i++) {
    QtDemuxStream *stream = qtdemux->streams[i];
    guint32 sample_num = 0;

    GST_DEBUG_OBJECT (qtdemux, "stream %d, id %d, fourcc %" GST_FOURCC_FORMAT,
        i, stream->track_id, GST_FOURCC_ARGS (stream->fourcc));

    if (qtdemux->fragmented) {
      /* need all moov samples first */
      GST_OBJECT_LOCK (qtdemux);
      while (stream->n_samples == 0)
        if ((ret = qtdemux_add_fragmented_samples (qtdemux)) != GST_FLOW_OK)
          break;
      GST_OBJECT_UNLOCK (qtdemux);
    } else {
      /* discard any stray moof */
      qtdemux->moof_offset = 0;
    }

    /* prepare braking */
    if (ret != GST_FLOW_ERROR)
      ret = GST_FLOW_OK;

    /* in pull mode, we should have parsed some sample info by now;
     * and quite some code will not handle no samples.
     * in push mode, we'll just have to deal with it */
    if (G_UNLIKELY (qtdemux->pullbased && !stream->n_samples)) {
      GST_DEBUG_OBJECT (qtdemux, "no samples for stream; discarding");
      gst_qtdemux_remove_stream (qtdemux, i);
      i--;
      continue;
    }

    /* parse the initial sample for use in setting the frame rate cap */
    while (sample_num == 0 && sample_num < stream->n_samples) {
      if (!qtdemux_parse_samples (qtdemux, stream, sample_num))
        break;
      ++sample_num;
    }
    if (stream->n_samples > 0 && stream->stbl_index >= 0) {
      stream->first_duration = stream->samples[0].duration;
      GST_LOG_OBJECT (qtdemux, "stream %d first duration %u",
          stream->track_id, stream->first_duration);
    }
  }

  return ret;
}