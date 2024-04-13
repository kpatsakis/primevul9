gst_date_time_get_time_zone_offset (const GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, 0.0);
  g_return_val_if_fail (gst_date_time_has_time (datetime), 0.0);

  return (g_date_time_get_utc_offset (datetime->datetime) /
      G_USEC_PER_SEC) / 3600.0;
}