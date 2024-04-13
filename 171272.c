gst_date_time_get_day (const GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, 0);
  g_return_val_if_fail (gst_date_time_has_day (datetime), 0);

  return g_date_time_get_day_of_month (datetime->datetime);
}