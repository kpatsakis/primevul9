qtdemux_sink_activate (GstPad * sinkpad, GstObject * parent)
{
  GstQuery *query;
  gboolean pull_mode;

  query = gst_query_new_scheduling ();

  if (!gst_pad_peer_query (sinkpad, query)) {
    gst_query_unref (query);
    goto activate_push;
  }

  pull_mode = gst_query_has_scheduling_mode_with_flags (query,
      GST_PAD_MODE_PULL, GST_SCHEDULING_FLAG_SEEKABLE);
  gst_query_unref (query);

  if (!pull_mode)
    goto activate_push;

  GST_DEBUG_OBJECT (sinkpad, "activating pull");
  return gst_pad_activate_mode (sinkpad, GST_PAD_MODE_PULL, TRUE);

activate_push:
  {
    GST_DEBUG_OBJECT (sinkpad, "activating push");
    return gst_pad_activate_mode (sinkpad, GST_PAD_MODE_PUSH, TRUE);
  }
}