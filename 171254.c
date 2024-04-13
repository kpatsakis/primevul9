GST_START_TEST (test_GstDateTime_new_from_unix_epoch_local_time)
{
  GstDateTime *dt;
  struct tm tm;
  time_t t;

  memset (&tm, 0, sizeof (tm));
  t = time (NULL);
#ifdef HAVE_LOCALTIME_R
  localtime_r (&t, &tm);
#else
  memcpy (&tm, localtime (&t), sizeof (struct tm));
#endif
  dt = gst_date_time_new_from_unix_epoch_local_time (t);
  assert_equals_int (gst_date_time_get_year (dt), 1900 + tm.tm_year);
  assert_equals_int (gst_date_time_get_month (dt), 1 + tm.tm_mon);
  assert_equals_int (gst_date_time_get_day (dt), tm.tm_mday);
  assert_equals_int (gst_date_time_get_hour (dt), tm.tm_hour);
  assert_equals_int (gst_date_time_get_minute (dt), tm.tm_min);
  assert_equals_int (gst_date_time_get_second (dt), tm.tm_sec);
  gst_date_time_unref (dt);

  memset (&tm, 0, sizeof (tm));
  tm.tm_year = 70;
  tm.tm_mday = 1;
  tm.tm_mon = 0;
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  t = mktime (&tm);
  dt = gst_date_time_new_from_unix_epoch_local_time (t);
  assert_equals_int (gst_date_time_get_year (dt), 1970);
  assert_equals_int (gst_date_time_get_month (dt), 1);
  assert_equals_int (gst_date_time_get_day (dt), 1);
  assert_equals_int (gst_date_time_get_hour (dt), 0);
  assert_equals_int (gst_date_time_get_minute (dt), 0);
  assert_equals_int (gst_date_time_get_second (dt), 0);
  gst_date_time_unref (dt);
}