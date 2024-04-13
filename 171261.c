date_times_are_equal (GstDateTime * d1, GstDateTime * d2)
{
  GValue val1 = G_VALUE_INIT;
  GValue val2 = G_VALUE_INIT;
  int ret;

  g_value_init (&val1, GST_TYPE_DATE_TIME);
  g_value_set_boxed (&val1, d1);
  g_value_init (&val2, GST_TYPE_DATE_TIME);
  g_value_set_boxed (&val2, d2);
  ret = gst_value_compare (&val1, &val2);
  g_value_unset (&val2);
  g_value_unset (&val1);

  return ret == GST_VALUE_EQUAL;
}