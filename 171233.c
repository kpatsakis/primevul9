__gst_date_time_compare (const GstDateTime * dt1, const GstDateTime * dt2)
{
  gint64 diff;

  /* we assume here that GST_DATE_TIME_FIELDS_YMD_HMS is the highest
   * resolution, and ignore microsecond differences on purpose for now */
  if (dt1->fields != dt2->fields)
    return GST_VALUE_UNORDERED;

  /* This will round down to nearest second, which is what we want. We're
   * not comparing microseconds on purpose here, since we're not
   * serialising them when doing new_utc_now() + to_string() */
  diff =
      g_date_time_to_unix (dt1->datetime) - g_date_time_to_unix (dt2->datetime);
  if (diff < 0)
    return GST_VALUE_LESS_THAN;
  else if (diff > 0)
    return GST_VALUE_GREATER_THAN;
  else
    return GST_VALUE_EQUAL;
}