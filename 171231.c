gst_date_time_new_ymd (gint year, gint month, gint day)
{
  return gst_date_time_new (0.0, year, month, day, -1, -1, -1);
}