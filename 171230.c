gst_date_time_suite (void)
{
  Suite *s = suite_create ("GstDateTime");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_GstDateTime_get_dmy);
  tcase_add_test (tc_chain, test_GstDateTime_get_hour);
  tcase_add_test (tc_chain, test_GstDateTime_get_microsecond);
  tcase_add_test (tc_chain, test_GstDateTime_get_minute);
  tcase_add_test (tc_chain, test_GstDateTime_get_second);
  tcase_add_test (tc_chain, test_GstDateTime_get_utc_offset);
  tcase_add_test (tc_chain, test_GstDateTime_new_from_unix_epoch_local_time);
  tcase_add_test (tc_chain, test_GstDateTime_new_from_unix_epoch_utc);
  tcase_add_test (tc_chain, test_GstDateTime_new_full);
  tcase_add_test (tc_chain, test_GstDateTime_now);
  tcase_add_test (tc_chain, test_GstDateTime_utc_now);
  tcase_add_test (tc_chain, test_GstDateTime_partial_fields);
  tcase_add_test (tc_chain, test_GstDateTime_iso8601);
  tcase_add_test (tc_chain, test_GstDateTime_to_g_date_time);
  tcase_add_test (tc_chain, test_GstDateTime_new_from_g_date_time);

  return s;
}