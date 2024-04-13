gst_qtdemux_reset (GstQTDemux * qtdemux, gboolean hard)
{
  gint n;

  GST_DEBUG_OBJECT (qtdemux, "Resetting demux");
  gst_pad_stop_task (qtdemux->sinkpad);

  if (hard || qtdemux->upstream_format_is_time) {
    qtdemux->state = QTDEMUX_STATE_INITIAL;
    qtdemux->neededbytes = 16;
    qtdemux->todrop = 0;
    qtdemux->pullbased = FALSE;
    qtdemux->posted_redirect = FALSE;
    qtdemux->first_mdat = -1;
    qtdemux->header_size = 0;
    qtdemux->mdatoffset = -1;
    qtdemux->restoredata_offset = -1;
    if (qtdemux->mdatbuffer)
      gst_buffer_unref (qtdemux->mdatbuffer);
    if (qtdemux->restoredata_buffer)
      gst_buffer_unref (qtdemux->restoredata_buffer);
    qtdemux->mdatbuffer = NULL;
    qtdemux->restoredata_buffer = NULL;
    qtdemux->mdatleft = 0;
    if (qtdemux->comp_brands)
      gst_buffer_unref (qtdemux->comp_brands);
    qtdemux->comp_brands = NULL;
    qtdemux->last_moov_offset = -1;
    if (qtdemux->moov_node)
      g_node_destroy (qtdemux->moov_node);
    qtdemux->moov_node = NULL;
    qtdemux->moov_node_compressed = NULL;
    if (qtdemux->tag_list)
      gst_mini_object_unref (GST_MINI_OBJECT_CAST (qtdemux->tag_list));
    qtdemux->tag_list = NULL;
#if 0
    if (qtdemux->element_index)
      gst_object_unref (qtdemux->element_index);
    qtdemux->element_index = NULL;
#endif
    qtdemux->major_brand = 0;
    if (qtdemux->pending_newsegment)
      gst_event_unref (qtdemux->pending_newsegment);
    qtdemux->pending_newsegment = NULL;
    qtdemux->upstream_format_is_time = FALSE;
    qtdemux->upstream_seekable = FALSE;
    qtdemux->upstream_size = 0;

    qtdemux->fragment_start = -1;
    qtdemux->fragment_start_offset = -1;
    qtdemux->duration = 0;
    qtdemux->moof_offset = 0;
    qtdemux->chapters_track_id = 0;
    qtdemux->have_group_id = FALSE;
    qtdemux->group_id = G_MAXUINT;

    g_queue_foreach (&qtdemux->protection_event_queue, (GFunc) gst_event_unref,
        NULL);
    g_queue_clear (&qtdemux->protection_event_queue);
  }
  qtdemux->offset = 0;
  gst_adapter_clear (qtdemux->adapter);
  gst_segment_init (&qtdemux->segment, GST_FORMAT_TIME);
  qtdemux->segment_seqnum = 0;

  if (hard) {
    for (n = 0; n < qtdemux->n_streams; n++) {
      gst_qtdemux_stream_free (qtdemux, qtdemux->streams[n]);
      qtdemux->streams[n] = NULL;
    }
    qtdemux->n_streams = 0;
    qtdemux->n_video_streams = 0;
    qtdemux->n_audio_streams = 0;
    qtdemux->n_sub_streams = 0;
    qtdemux->exposed = FALSE;
    qtdemux->fragmented = FALSE;
    qtdemux->mss_mode = FALSE;
    gst_caps_replace (&qtdemux->media_caps, NULL);
    qtdemux->timescale = 0;
    qtdemux->got_moov = FALSE;
    if (qtdemux->protection_system_ids) {
      g_ptr_array_free (qtdemux->protection_system_ids, TRUE);
      qtdemux->protection_system_ids = NULL;
    }
  } else if (qtdemux->mss_mode) {
    gst_flow_combiner_reset (qtdemux->flowcombiner);
    for (n = 0; n < qtdemux->n_streams; n++)
      gst_qtdemux_stream_clear (qtdemux, qtdemux->streams[n]);
  } else {
    gst_flow_combiner_reset (qtdemux->flowcombiner);
    for (n = 0; n < qtdemux->n_streams; n++) {
      qtdemux->streams[n]->sent_eos = FALSE;
      qtdemux->streams[n]->time_position = 0;
      qtdemux->streams[n]->accumulated_base = 0;
    }
    if (!qtdemux->pending_newsegment) {
      qtdemux->pending_newsegment = gst_event_new_segment (&qtdemux->segment);
      if (qtdemux->segment_seqnum)
        gst_event_set_seqnum (qtdemux->pending_newsegment,
            qtdemux->segment_seqnum);
    }
  }
}