GST_START_TEST (test_GstDateTime_new_full)
{
  GstDateTime *dt;

  dt = gst_date_time_new (0, 2009, 12, 11, 12, 11, 10.001234);
  assert_equals_int (2009, gst_date_time_get_year (dt));
  assert_equals_int (12, gst_date_time_get_month (dt));
  assert_equals_int (11, gst_date_time_get_day (dt));
  assert_equals_int (12, gst_date_time_get_hour (dt));
  assert_equals_int (11, gst_date_time_get_minute (dt));
  assert_equals_int (10, gst_date_time_get_second (dt));
  assert_equals_int (1234, gst_date_time_get_microsecond (dt));
  assert_equals_float (0, gst_date_time_get_time_zone_offset (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (2.5, 2010, 3, 29, 12, 13, 16.5);
  assert_equals_int (2010, gst_date_time_get_year (dt));
  assert_equals_int (3, gst_date_time_get_month (dt));
  assert_equals_int (29, gst_date_time_get_day (dt));
  assert_equals_int (12, gst_date_time_get_hour (dt));
  assert_equals_int (13, gst_date_time_get_minute (dt));
  assert_equals_int (16, gst_date_time_get_second (dt));
  assert_equals_int (500000, gst_date_time_get_microsecond (dt));
  assert_equals_float (2.5, gst_date_time_get_time_zone_offset (dt));
  gst_date_time_unref (dt);
}