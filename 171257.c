gst_date_time_new_ym (gint year, gint month)
{
  return gst_date_time_new (0.0, year, month, -1, -1, -1, -1);
}