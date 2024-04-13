qtdemux_sink_activate_mode (GstPad * sinkpad, GstObject * parent,
    GstPadMode mode, gboolean active)
{
  gboolean res;
  GstQTDemux *demux = GST_QTDEMUX (parent);

  switch (mode) {
    case GST_PAD_MODE_PUSH:
      demux->pullbased = FALSE;
      res = TRUE;
      break;
    case GST_PAD_MODE_PULL:
      if (active) {
        demux->pullbased = TRUE;
        res = gst_pad_start_task (sinkpad, (GstTaskFunction) gst_qtdemux_loop,
            sinkpad, NULL);
      } else {
        res = gst_pad_stop_task (sinkpad);
      }
      break;
    default:
      res = FALSE;
      break;
  }
  return res;
}