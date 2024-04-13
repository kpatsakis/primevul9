gst_date_time_new (gfloat tzoffset, gint year, gint month, gint day, gint hour,
    gint minute, gdouble seconds)
{
  GstDateTimeFields fields;
  gchar buf[6];
  GTimeZone *tz;
  GDateTime *dt;
  GstDateTime *datetime;
  gint tzhour, tzminute;

  g_return_val_if_fail (year > 0 && year <= 9999, NULL);
  g_return_val_if_fail ((month > 0 && month <= 12) || month == -1, NULL);
  g_return_val_if_fail ((day > 0 && day <= 31) || day == -1, NULL);
  g_return_val_if_fail ((hour >= 0 && hour < 24) || hour == -1, NULL);
  g_return_val_if_fail ((minute >= 0 && minute < 60) || minute == -1, NULL);
  g_return_val_if_fail ((seconds >= 0 && seconds < 60) || seconds == -1, NULL);
  g_return_val_if_fail (tzoffset >= -12.0 && tzoffset <= 12.0, NULL);
  g_return_val_if_fail ((hour >= 0 && minute >= 0) ||
      (hour == -1 && minute == -1 && seconds == -1 && tzoffset == 0.0), NULL);

  tzhour = (gint) ABS (tzoffset);
  tzminute = (gint) ((ABS (tzoffset) - tzhour) * 60);

  g_snprintf (buf, 6, "%c%02d%02d", tzoffset >= 0 ? '+' : '-', tzhour,
      tzminute);

  tz = g_time_zone_new (buf);

  fields = gst_date_time_check_fields (&year, &month, &day,
      &hour, &minute, &seconds);

  dt = g_date_time_new (tz, year, month, day, hour, minute, seconds);
  g_time_zone_unref (tz);

  datetime = gst_date_time_new_from_g_date_time (dt);
  datetime->fields = fields;

  return datetime;
}