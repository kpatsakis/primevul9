gst_date_time_new_from_unix_epoch_utc (gint64 secs)
{
  GstDateTime *datetime;
  datetime =
      gst_date_time_new_from_g_date_time (g_date_time_new_from_unix_utc (secs));
  return datetime;
}