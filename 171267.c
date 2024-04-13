GST_START_TEST (test_GstDateTime_get_dmy)
{
  GstDateTime *dt;
  time_t t;
  struct tm tt;

  t = time (NULL);
#ifdef HAVE_LOCALTIME_R
  localtime_r (&t, &tt);
#else
  memcpy (&tt, localtime (&t), sizeof (struct tm));
#endif
  dt = gst_date_time_new_from_unix_epoch_local_time (t);
  assert_equals_int (gst_date_time_get_year (dt), tt.tm_year + 1900);
  assert_equals_int (gst_date_time_get_month (dt), tt.tm_mon + 1);
  assert_equals_int (gst_date_time_get_day (dt), tt.tm_mday);

  gst_date_time_unref (dt);
}