GST_START_TEST (test_GstDateTime_utc_now)
{
  GstDateTime *dt;
  time_t t;
  struct tm tm;

  t = time (NULL);
#ifdef HAVE_GMTIME_R
  gmtime_r (&t, &tm);
#else
  memcpy (&tm, gmtime (&t), sizeof (struct tm));
#endif
  dt = gst_date_time_new_now_utc ();
  assert_equals_int (tm.tm_year + 1900, gst_date_time_get_year (dt));
  assert_equals_int (tm.tm_mon + 1, gst_date_time_get_month (dt));
  assert_equals_int (tm.tm_mday, gst_date_time_get_day (dt));
  assert_equals_int (tm.tm_hour, gst_date_time_get_hour (dt));
  assert_equals_int (tm.tm_min, gst_date_time_get_minute (dt));
  assert_almost_equals_int (tm.tm_sec, gst_date_time_get_second (dt));
  gst_date_time_unref (dt);
}