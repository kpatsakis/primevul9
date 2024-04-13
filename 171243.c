gst_date_time_new_from_unix_epoch_local_time (gint64 secs)
{
  GDateTime *datetime;

  datetime = g_date_time_new_from_unix_local (secs);
  return gst_date_time_new_from_g_date_time (datetime);
}