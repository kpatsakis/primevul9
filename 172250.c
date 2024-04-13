START_TEST(test_tm_poll_bad_init)
  {
  fail_unless(TM_BADINIT == tm_poll(0, 0, 0, 0), "expected poll to return TM_BADINIT if no init called first.");
  }