GST_START_TEST (test_GstDateTime_now)
{
  GstDateTime *dt;
  time_t t;
  struct tm tm;

  memset (&tm, 0, sizeof (tm));
  t = time (NULL);
#ifdef HAVE_LOCALTIME_R
  localtime_r (&t, &tm);
#else
  memcpy (&tm, localtime (&t), sizeof (struct tm));
#endif
  dt = gst_date_time_new_now_local_time ();
  assert_equals_int (gst_date_time_get_year (dt), 1900 + tm.tm_year);
  assert_equals_int (gst_date_time_get_month (dt), 1 + tm.tm_mon);
  assert_equals_int (gst_date_time_get_day (dt), tm.tm_mday);
  assert_equals_int (gst_date_time_get_hour (dt), tm.tm_hour);
  assert_equals_int (gst_date_time_get_minute (dt), tm.tm_min);
  assert_almost_equals_int (gst_date_time_get_second (dt), tm.tm_sec);
  gst_date_time_unref (dt);
}