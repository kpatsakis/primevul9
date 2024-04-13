gst_date_time_get_year (const GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, 0);

  return g_date_time_get_year (datetime->datetime);
}