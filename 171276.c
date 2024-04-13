GST_START_TEST (test_GstDateTime_get_minute)
{
  GstDateTime *dt;

  dt = gst_date_time_new (0, 2009, 12, 1, 1, 31, 0);
  assert_equals_int (31, gst_date_time_get_minute (dt));
  gst_date_time_unref (dt);
}