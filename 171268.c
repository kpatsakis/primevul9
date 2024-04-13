GST_START_TEST (test_GstDateTime_new_from_g_date_time)
{
  GDateTime *gdt;
  GstDateTime *dt;

  gdt = g_date_time_new_now_utc ();
  g_date_time_ref (gdt);        /* keep it alive for compare below */
  dt = gst_date_time_new_from_g_date_time (gdt);

  assert_equals_int (gst_date_time_get_year (dt), g_date_time_get_year (gdt));
  assert_equals_int (gst_date_time_get_month (dt), g_date_time_get_month (gdt));
  assert_equals_int (gst_date_time_get_day (dt),
      g_date_time_get_day_of_month (gdt));
  assert_equals_int (gst_date_time_get_hour (dt), g_date_time_get_hour (gdt));
  assert_equals_int (gst_date_time_get_minute (dt),
      g_date_time_get_minute (gdt));
  assert_equals_int (gst_date_time_get_second (dt),
      g_date_time_get_second (gdt));
  assert_equals_int (gst_date_time_get_microsecond (dt),
      g_date_time_get_microsecond (gdt));

  g_date_time_unref (gdt);
  gst_date_time_unref (dt);
}