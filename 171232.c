gst_date_time_new_local_time (gint year, gint month, gint day, gint hour,
    gint minute, gdouble seconds)
{
  GstDateTimeFields fields;
  GstDateTime *datetime;

  g_return_val_if_fail (year > 0 && year <= 9999, NULL);
  g_return_val_if_fail ((month > 0 && month <= 12) || month == -1, NULL);
  g_return_val_if_fail ((day > 0 && day <= 31) || day == -1, NULL);
  g_return_val_if_fail ((hour >= 0 && hour < 24) || hour == -1, NULL);
  g_return_val_if_fail ((minute >= 0 && minute < 60) || minute == -1, NULL);
  g_return_val_if_fail ((seconds >= 0 && seconds < 60) || seconds == -1, NULL);

  fields = gst_date_time_check_fields (&year, &month, &day,
      &hour, &minute, &seconds);

  datetime = gst_date_time_new_from_g_date_time (g_date_time_new_local (year,
          month, day, hour, minute, seconds));

  datetime->fields = fields;
  return datetime;
}