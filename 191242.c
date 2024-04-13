gst_qtdemux_convert_seek (GstPad * pad, GstFormat * format,
    GstSeekType cur_type, gint64 * cur, GstSeekType stop_type, gint64 * stop)
{
  gboolean res;

  g_return_val_if_fail (format != NULL, FALSE);
  g_return_val_if_fail (cur != NULL, FALSE);
  g_return_val_if_fail (stop != NULL, FALSE);

  if (*format == GST_FORMAT_TIME)
    return TRUE;

  res = TRUE;
  if (cur_type != GST_SEEK_TYPE_NONE)
    res = gst_pad_query_convert (pad, *format, *cur, GST_FORMAT_TIME, cur);
  if (res && stop_type != GST_SEEK_TYPE_NONE)
    res = gst_pad_query_convert (pad, *format, *stop, GST_FORMAT_TIME, stop);

  if (res)
    *format = GST_FORMAT_TIME;

  return res;
}