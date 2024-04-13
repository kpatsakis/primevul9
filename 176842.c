gst_matroska_demux_sink_activate (GstPad * sinkpad, GstObject * parent)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (parent);
  GstQuery *query;
  gboolean pull_mode = FALSE;

  query = gst_query_new_scheduling ();

  if (gst_pad_peer_query (sinkpad, query))
    pull_mode = gst_query_has_scheduling_mode_with_flags (query,
        GST_PAD_MODE_PULL, GST_SCHEDULING_FLAG_SEEKABLE);

  gst_query_unref (query);

  if (pull_mode) {
    GST_DEBUG ("going to pull mode");
    demux->streaming = FALSE;
    return gst_pad_activate_mode (sinkpad, GST_PAD_MODE_PULL, TRUE);
  } else {
    GST_DEBUG ("going to push (streaming) mode");
    demux->streaming = TRUE;
    return gst_pad_activate_mode (sinkpad, GST_PAD_MODE_PUSH, TRUE);
  }
}