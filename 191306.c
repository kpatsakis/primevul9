gst_qtdemux_init (GstQTDemux * qtdemux)
{
  qtdemux->sinkpad =
      gst_pad_new_from_static_template (&gst_qtdemux_sink_template, "sink");
  gst_pad_set_activate_function (qtdemux->sinkpad, qtdemux_sink_activate);
  gst_pad_set_activatemode_function (qtdemux->sinkpad,
      qtdemux_sink_activate_mode);
  gst_pad_set_chain_function (qtdemux->sinkpad, gst_qtdemux_chain);
  gst_pad_set_event_function (qtdemux->sinkpad, gst_qtdemux_handle_sink_event);
  gst_element_add_pad (GST_ELEMENT_CAST (qtdemux), qtdemux->sinkpad);

  qtdemux->state = QTDEMUX_STATE_INITIAL;
  qtdemux->pullbased = FALSE;
  qtdemux->posted_redirect = FALSE;
  qtdemux->neededbytes = 16;
  qtdemux->todrop = 0;
  qtdemux->adapter = gst_adapter_new ();
  qtdemux->offset = 0;
  qtdemux->first_mdat = -1;
  qtdemux->got_moov = FALSE;
  qtdemux->mdatoffset = -1;
  qtdemux->mdatbuffer = NULL;
  qtdemux->restoredata_buffer = NULL;
  qtdemux->restoredata_offset = -1;
  qtdemux->fragment_start = -1;
  qtdemux->fragment_start_offset = -1;
  qtdemux->media_caps = NULL;
  qtdemux->exposed = FALSE;
  qtdemux->mss_mode = FALSE;
  qtdemux->pending_newsegment = NULL;
  qtdemux->upstream_format_is_time = FALSE;
  qtdemux->have_group_id = FALSE;
  qtdemux->group_id = G_MAXUINT;
  qtdemux->cenc_aux_info_offset = 0;
  qtdemux->cenc_aux_info_sizes = NULL;
  qtdemux->cenc_aux_sample_count = 0;
  qtdemux->protection_system_ids = NULL;
  g_queue_init (&qtdemux->protection_event_queue);
  gst_segment_init (&qtdemux->segment, GST_FORMAT_TIME);
  qtdemux->flowcombiner = gst_flow_combiner_new ();

  GST_OBJECT_FLAG_SET (qtdemux, GST_ELEMENT_FLAG_INDEXABLE);
}