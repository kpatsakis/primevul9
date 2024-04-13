GST_START_TEST (test_GstDateTime_partial_fields)
{
  GstDateTime *dt;

  ASSERT_CRITICAL (gst_date_time_new (0.0, -1, -1, -1, -1, -1, -1));
  ASSERT_CRITICAL (gst_date_time_new (0.0, 2012, 7, 18, 9, -1, -1));

  dt = gst_date_time_new (0.0, 2012, -1, -1, -1, -1, -1);
  fail_unless (gst_date_time_has_year (dt));
  fail_unless_equals_int (gst_date_time_get_year (dt), 2012);
  fail_if (gst_date_time_has_month (dt));
  ASSERT_CRITICAL (gst_date_time_get_month (dt));
  fail_if (gst_date_time_has_day (dt));
  ASSERT_CRITICAL (gst_date_time_get_day (dt));
  fail_if (gst_date_time_has_time (dt));
  ASSERT_CRITICAL (gst_date_time_get_hour (dt));
  ASSERT_CRITICAL (gst_date_time_get_minute (dt));
  fail_if (gst_date_time_has_second (dt));
  ASSERT_CRITICAL (gst_date_time_get_second (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0.0, 2012, 7, -1, -1, -1, -1);
  fail_unless (gst_date_time_has_year (dt));
  fail_unless_equals_int (gst_date_time_get_year (dt), 2012);
  fail_unless (gst_date_time_has_month (dt));
  fail_unless_equals_int (gst_date_time_get_month (dt), 7);
  fail_if (gst_date_time_has_day (dt));
  ASSERT_CRITICAL (gst_date_time_get_day (dt));
  fail_if (gst_date_time_has_time (dt));
  ASSERT_CRITICAL (gst_date_time_get_hour (dt));
  ASSERT_CRITICAL (gst_date_time_get_minute (dt));
  fail_if (gst_date_time_has_second (dt));
  ASSERT_CRITICAL (gst_date_time_get_second (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0.0, 2012, 7, 1, -1, -1, -1);
  fail_unless (gst_date_time_has_year (dt));
  fail_unless (gst_date_time_has_month (dt));
  fail_unless_equals_int (gst_date_time_get_month (dt), 7);
  fail_unless (gst_date_time_has_day (dt));
  fail_unless_equals_int (gst_date_time_get_day (dt), 1);
  fail_if (gst_date_time_has_time (dt));
  fail_if (gst_date_time_has_second (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0.0, 2012, 7, 1, 18, 20, -1);
  fail_unless (gst_date_time_has_year (dt));
  fail_unless_equals_int (gst_date_time_get_year (dt), 2012);
  fail_unless (gst_date_time_has_month (dt));
  fail_unless_equals_int (gst_date_time_get_month (dt), 7);
  fail_unless (gst_date_time_has_day (dt));
  fail_unless_equals_int (gst_date_time_get_day (dt), 1);
  fail_unless (gst_date_time_has_time (dt));
  fail_unless_equals_int (gst_date_time_get_hour (dt), 18);
  fail_unless_equals_int (gst_date_time_get_minute (dt), 20);
  fail_if (gst_date_time_has_second (dt));
  gst_date_time_unref (dt);

  dt = gst_date_time_new (0.0, 2012, 7, 1, 18, 20, 25.0443);
  fail_unless (gst_date_time_has_year (dt));
  fail_unless (gst_date_time_has_month (dt));
  fail_unless (gst_date_time_has_day (dt));
  fail_unless (gst_date_time_has_time (dt));
  fail_unless (gst_date_time_has_second (dt));
  fail_unless_equals_int (gst_date_time_get_second (dt), 25);
  /* fail_unless_equals_int (gst_date_time_get_microsecond (dt), 443); */
  gst_date_time_unref (dt);
}