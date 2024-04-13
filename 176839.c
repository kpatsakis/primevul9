gst_matroska_demux_seek_to_previous_keyframe (GstMatroskaDemux * demux)
{
  GstFlowReturn ret = GST_FLOW_EOS;
  gboolean done = TRUE;
  gint i;

  g_return_val_if_fail (demux->seek_index, GST_FLOW_EOS);
  g_return_val_if_fail (demux->seek_entry < demux->seek_index->len,
      GST_FLOW_EOS);

  GST_DEBUG_OBJECT (demux, "locating previous keyframe");

  if (!demux->seek_entry) {
    GST_DEBUG_OBJECT (demux, "no earlier index entry");
    goto exit;
  }

  for (i = 0; i < demux->common.src->len; i++) {
    GstMatroskaTrackContext *stream = g_ptr_array_index (demux->common.src, i);

    GST_DEBUG_OBJECT (demux, "segment start %" GST_TIME_FORMAT
        ", stream %d at %" GST_TIME_FORMAT,
        GST_TIME_ARGS (demux->common.segment.start), stream->index,
        GST_TIME_ARGS (stream->from_time));
    if (GST_CLOCK_TIME_IS_VALID (stream->from_time)) {
      if (stream->from_time > demux->common.segment.start) {
        GST_DEBUG_OBJECT (demux, "stream %d not finished yet", stream->index);
        done = FALSE;
      }
    } else {
      /* nothing pushed for this stream;
       * likely seek entry did not start at keyframe, so all was skipped.
       * So we need an earlier entry */
      done = FALSE;
    }
  }

  if (!done) {
    GstMatroskaIndex *entry;

    entry = &g_array_index (demux->seek_index, GstMatroskaIndex,
        --demux->seek_entry);
    if (!gst_matroska_demux_move_to_entry (demux, entry, FALSE, TRUE))
      goto exit;

    ret = GST_FLOW_OK;
  }

exit:
  return ret;
}