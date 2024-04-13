gst_date_time_new_from_g_date_time (GDateTime * dt)
{
  GstDateTime *gst_dt;

  if (!dt)
    return NULL;

  gst_dt = g_slice_new (GstDateTime);

  gst_mini_object_init (GST_MINI_OBJECT_CAST (gst_dt), 0, GST_TYPE_DATE_TIME,
      NULL, NULL, (GstMiniObjectFreeFunction) gst_date_time_free);

  gst_dt->datetime = dt;
  gst_dt->fields = GST_DATE_TIME_FIELDS_YMD_HMS;
  return gst_dt;
}