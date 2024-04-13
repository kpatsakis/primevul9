gst_matroska_demux_change_state (GstElement * element,
    GstStateChange transition)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (element);
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  /* handle upwards state changes here */
  switch (transition) {
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  /* handle downwards state changes */
  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      gst_matroska_demux_reset (GST_ELEMENT (demux));
      break;
    default:
      break;
  }

  return ret;
}