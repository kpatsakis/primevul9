GST_START_TEST (test_GstDateTime_get_hour)
{
  GstDateTime *dt;

  dt = gst_date_time_new (0, 2009, 10, 19, 15, 13, 11);
  assert_equals_int (15, gst_date_time_get_hour (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0, 100, 10, 19, 1, 0, 0);
  assert_equals_int (1, gst_date_time_get_hour (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0, 100, 10, 19, 0, 0, 0);
  assert_equals_int (0, gst_date_time_get_hour (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0, 100, 10, 1, 23, 59, 59);
  assert_equals_int (23, gst_date_time_get_hour (dt));
  gst_date_time_unref (dt);
}