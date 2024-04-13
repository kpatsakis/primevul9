gst_date_time_free (GstDateTime * datetime)
{
  g_date_time_unref (datetime->datetime);
  g_slice_free (GstDateTime, datetime);
}