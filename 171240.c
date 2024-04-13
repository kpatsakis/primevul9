gst_date_time_get_month (const GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, 0);
  g_return_val_if_fail (gst_date_time_has_month (datetime), 0);

  return g_date_time_get_month (datetime->datetime);
}