gst_matroska_demux_reset (GstElement * element)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (element);

  GST_DEBUG_OBJECT (demux, "Resetting state");

  gst_matroska_read_common_reset (GST_ELEMENT (demux), &demux->common);

  demux->num_a_streams = 0;
  demux->num_t_streams = 0;
  demux->num_v_streams = 0;
  demux->have_nonintraonly_v_streams = FALSE;

  demux->have_group_id = FALSE;
  demux->group_id = G_MAXUINT;

  demux->clock = NULL;
  demux->tracks_parsed = FALSE;

  if (demux->clusters) {
    g_array_free (demux->clusters, TRUE);
    demux->clusters = NULL;
  }

  g_list_foreach (demux->seek_parsed,
      (GFunc) gst_matroska_read_common_free_parsed_el, NULL);
  g_list_free (demux->seek_parsed);
  demux->seek_parsed = NULL;

  demux->last_stop_end = GST_CLOCK_TIME_NONE;
  demux->seek_block = 0;
  demux->stream_start_time = GST_CLOCK_TIME_NONE;
  demux->to_time = GST_CLOCK_TIME_NONE;
  demux->cluster_time = GST_CLOCK_TIME_NONE;
  demux->cluster_offset = 0;
  demux->cluster_prevsize = 0;
  demux->seen_cluster_prevsize = FALSE;
  demux->next_cluster_offset = 0;
  demux->stream_last_time = GST_CLOCK_TIME_NONE;
  demux->last_cluster_offset = 0;
  demux->index_offset = 0;
  demux->seekable = FALSE;
  demux->need_segment = FALSE;
  demux->segment_seqnum = 0;
  demux->requested_seek_time = GST_CLOCK_TIME_NONE;
  demux->seek_offset = -1;
  demux->audio_lead_in_ts = 0;
  demux->building_index = FALSE;
  if (demux->seek_event) {
    gst_event_unref (demux->seek_event);
    demux->seek_event = NULL;
  }

  demux->seek_index = NULL;
  demux->seek_entry = 0;

  if (demux->new_segment) {
    gst_event_unref (demux->new_segment);
    demux->new_segment = NULL;
  }

  demux->invalid_duration = FALSE;

  demux->cached_length = G_MAXUINT64;

  if (demux->deferred_seek_event)
    gst_event_unref (demux->deferred_seek_event);
  demux->deferred_seek_event = NULL;
  demux->deferred_seek_pad = NULL;

  gst_flow_combiner_clear (demux->flowcombiner);
}