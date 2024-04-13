gst_matroska_demux_move_to_entry (GstMatroskaDemux * demux,
    GstMatroskaIndex * entry, gboolean reset, gboolean update)
{
  gint i;

  GST_OBJECT_LOCK (demux);

  if (update) {
    /* seek (relative to matroska segment) */
    /* position might be invalid; will error when streaming resumes ... */
    demux->common.offset = entry->pos + demux->common.ebml_segment_start;
    demux->next_cluster_offset = 0;

    GST_DEBUG_OBJECT (demux,
        "Seeked to offset %" G_GUINT64_FORMAT ", block %d, " "time %"
        GST_TIME_FORMAT, entry->pos + demux->common.ebml_segment_start,
        entry->block, GST_TIME_ARGS (entry->time));

    /* update the time */
    gst_matroska_read_common_reset_streams (&demux->common, entry->time, TRUE);
    gst_flow_combiner_reset (demux->flowcombiner);
    demux->common.segment.position = entry->time;
    demux->seek_block = entry->block;
    demux->seek_first = TRUE;
    demux->last_stop_end = GST_CLOCK_TIME_NONE;
  }

  for (i = 0; i < demux->common.src->len; i++) {
    GstMatroskaTrackContext *stream = g_ptr_array_index (demux->common.src, i);

    if (reset) {
      stream->to_offset = G_MAXINT64;
    } else {
      if (stream->from_offset != -1)
        stream->to_offset = stream->from_offset;
    }
    stream->from_offset = -1;
    stream->from_time = GST_CLOCK_TIME_NONE;
  }

  GST_OBJECT_UNLOCK (demux);

  return TRUE;
}