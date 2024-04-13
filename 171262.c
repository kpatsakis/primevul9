gst_date_time_to_iso8601_string (GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, NULL);

  if (datetime->fields == GST_DATE_TIME_FIELDS_INVALID)
    return NULL;

  return __gst_date_time_serialize (datetime, FALSE);
}