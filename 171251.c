GST_START_TEST (test_GstDateTime_get_second)
{
  GstDateTime *dt;

  dt = gst_date_time_new (0, 2009, 12, 1, 1, 31, 44);
  assert_equals_int (44, gst_date_time_get_second (dt));
  gst_date_time_unref (dt);
}