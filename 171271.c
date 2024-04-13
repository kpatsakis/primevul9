GST_START_TEST (test_GstDateTime_new_from_unix_epoch_utc)
{
  GstDateTime *dt;
  struct tm tm;
  time_t t;

  memset (&tm, 0, sizeof (tm));
  t = time (NULL);
#ifdef HAVE_GMTIME_R
  gmtime_r (&t, &tm);
#else
  memcpy (&tm, gmtime (&t), sizeof (struct tm));
#endif
  dt = gst_date_time_new_from_unix_epoch_utc (t);
  assert_equals_int (gst_date_time_get_year (dt), 1900 + tm.tm_year);
  assert_equals_int (gst_date_time_get_month (dt), 1 + tm.tm_mon);
  assert_equals_int (gst_date_time_get_day (dt), tm.tm_mday);
  assert_equals_int (gst_date_time_get_hour (dt), tm.tm_hour);
  assert_equals_int (gst_date_time_get_minute (dt), tm.tm_min);
  assert_equals_int (gst_date_time_get_second (dt), tm.tm_sec);
  assert_equals_int (gst_date_time_get_time_zone_offset (dt), 0);
  gst_date_time_unref (dt);
}