gst_date_time_has_month (const GstDateTime * datetime)
{
  g_return_val_if_fail (datetime != NULL, FALSE);

  return (datetime->fields >= GST_DATE_TIME_FIELDS_YM);
}