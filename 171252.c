GST_START_TEST (test_GstDateTime_get_utc_offset)
{
  struct tm tm;
  time_t t;

  t = time (NULL);
  memset (&tm, 0, sizeof (tm));
#ifdef HAVE_LOCALTIME_R
  localtime_r (&t, &tm);
#else
  memcpy (&tm, localtime (&t), sizeof (struct tm));
#endif

#ifdef HAVE_TM_GMTOFF
  {
    GstDateTime *dt;
    gfloat ts;

    dt = gst_date_time_new_now_local_time ();
    ts = gst_date_time_get_time_zone_offset (dt);
    assert_equals_int (ts, tm.tm_gmtoff / 3600.0);
    gst_date_time_unref (dt);
  }
#endif
}