GST_START_TEST (test_GstDateTime_get_microsecond)
{
  GTimeVal tv;
  GstDateTime *dt;

  g_get_current_time (&tv);
  dt = gst_date_time_new (0, 2010, 7, 15, 11, 12,
      13 + (tv.tv_usec / 1000000.0));
  assert_almost_equals_int (tv.tv_usec, gst_date_time_get_microsecond (dt));
  gst_date_time_unref (dt);
}