gst_date_time_new_now_local_time (void)
{
  return gst_date_time_new_from_g_date_time (g_date_time_new_now_local ());
}