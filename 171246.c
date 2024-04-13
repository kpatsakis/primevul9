GST_START_TEST (test_GstDateTime_to_g_date_time)
{
  GDateTime *gdt1;
  GDateTime *gdt2;
  GstDateTime *dt;

  gdt1 = g_date_time_new_now_utc ();
  g_date_time_ref (gdt1);       /* keep it alive for compare below */
  dt = gst_date_time_new_from_g_date_time (gdt1);
  gdt2 = gst_date_time_to_g_date_time (dt);

  fail_unless (g_date_time_compare (gdt1, gdt2) == 0);

  g_date_time_unref (gdt1);
  g_date_time_unref (gdt2);
  gst_date_time_unref (dt);
}