__gst_date_time_serialize (GstDateTime * datetime, gboolean serialize_usecs)
{
  GString *s;
  gfloat gmt_offset;
  guint msecs;

  /* we always have at least the year */
  s = g_string_new (NULL);
  g_string_append_printf (s, "%04u", gst_date_time_get_year (datetime));

  if (datetime->fields == GST_DATE_TIME_FIELDS_Y)
    goto done;

  /* add month */
  g_string_append_printf (s, "-%02u", gst_date_time_get_month (datetime));

  if (datetime->fields == GST_DATE_TIME_FIELDS_YM)
    goto done;

  /* add day of month */
  g_string_append_printf (s, "-%02u", gst_date_time_get_day (datetime));

  if (datetime->fields == GST_DATE_TIME_FIELDS_YMD)
    goto done;

  /* add time */
  g_string_append_printf (s, "T%02u:%02u", gst_date_time_get_hour (datetime),
      gst_date_time_get_minute (datetime));

  if (datetime->fields == GST_DATE_TIME_FIELDS_YMD_HM)
    goto add_timezone;

  /* add seconds */
  g_string_append_printf (s, ":%02u", gst_date_time_get_second (datetime));

  /* add microseconds */
  if (serialize_usecs) {
    msecs = gst_date_time_get_microsecond (datetime);
    if (msecs != 0) {
      g_string_append_printf (s, ".%06u", msecs);
      /* trim trailing 0s */
      while (s->str[s->len - 1] == '0')
        g_string_truncate (s, s->len - 1);
    }
  }

  /* add timezone */

add_timezone:

  gmt_offset = gst_date_time_get_time_zone_offset (datetime);
  if (gmt_offset == 0) {
    g_string_append_c (s, 'Z');
  } else {
    guint tzhour, tzminute;

    tzhour = (guint) ABS (gmt_offset);
    tzminute = (guint) ((ABS (gmt_offset) - tzhour) * 60);

    g_string_append_c (s, (gmt_offset >= 0) ? '+' : '-');
    g_string_append_printf (s, "%02u%02u", tzhour, tzminute);
  }

done:

  return g_string_free (s, FALSE);
}