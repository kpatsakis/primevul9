gst_qtdemux_get_duration (GstQTDemux * qtdemux, GstClockTime * duration)
{
  gboolean res = FALSE;

  *duration = GST_CLOCK_TIME_NONE;

  if (qtdemux->duration != 0 &&
      qtdemux->duration != G_MAXINT64 && qtdemux->timescale != 0) {
    *duration = QTTIME_TO_GSTTIME (qtdemux, qtdemux->duration);
    res = TRUE;
  } else {
    *duration = GST_CLOCK_TIME_NONE;
  }

  return res;
}