gst_date_time_to_g_date_time (GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, NULL);

  if (datetime->fields != GST_DATE_TIME_FIELDS_YMD_HMS)
    return NULL;

  return g_date_time_add (datetime->datetime, 0);
}