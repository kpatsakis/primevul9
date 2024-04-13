gst_date_time_ref (GstDateTime * datetime)
{
  return (GstDateTime *) gst_mini_object_ref (GST_MINI_OBJECT_CAST (datetime));
}