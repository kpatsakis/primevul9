gst_date_time_get_minute (const GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, 0);
  g_return_val_if_fail (gst_date_time_has_time (datetime), 0);

  return g_date_time_get_minute (datetime->datetime);
}