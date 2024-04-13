gst_matroska_demux_sink_activate_mode (GstPad * sinkpad, GstObject * parent,
    GstPadMode mode, gboolean active)
{
  switch (mode) {
    case GST_PAD_MODE_PULL:
      if (active) {
        /* if we have a scheduler we can start the task */
        gst_pad_start_task (sinkpad, (GstTaskFunction) gst_matroska_demux_loop,
            sinkpad, NULL);
      } else {
        gst_pad_stop_task (sinkpad);
      }
      return TRUE;
    case GST_PAD_MODE_PUSH:
      return TRUE;
    default:
      return FALSE;
  }
}