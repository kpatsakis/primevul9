gst_date_time_unref (GstDateTime * datetime)
{
  gst_mini_object_unref (GST_MINI_OBJECT_CAST (datetime));
}