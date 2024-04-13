gst_matroska_demux_init (GstMatroskaDemux * demux)
{
  demux->common.sinkpad = gst_pad_new_from_static_template (&sink_templ,
      "sink");
  gst_pad_set_activate_function (demux->common.sinkpad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_sink_activate));
  gst_pad_set_activatemode_function (demux->common.sinkpad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_sink_activate_mode));
  gst_pad_set_chain_function (demux->common.sinkpad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_chain));
  gst_pad_set_event_function (demux->common.sinkpad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_handle_sink_event));
  gst_pad_set_query_function (demux->common.sinkpad,
      GST_DEBUG_FUNCPTR (gst_matroska_demux_handle_sink_query));
  gst_element_add_pad (GST_ELEMENT (demux), demux->common.sinkpad);

  /* init defaults for common read context */
  gst_matroska_read_common_init (&demux->common);

  /* property defaults */
  demux->max_gap_time = DEFAULT_MAX_GAP_TIME;
  demux->max_backtrack_distance = DEFAULT_MAX_BACKTRACK_DISTANCE;

  GST_OBJECT_FLAG_SET (demux, GST_ELEMENT_FLAG_INDEXABLE);

  demux->flowcombiner = gst_flow_combiner_new ();

  /* finish off */
  gst_matroska_demux_reset (GST_ELEMENT (demux));
}