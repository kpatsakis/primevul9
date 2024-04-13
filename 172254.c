START_TEST(test_tm_poll_bad_result)
  {
  struct tm_roots roots;
  fake_tm_init(NULL, &roots);
  fail_unless(TM_EBADENVIRONMENT == tm_poll(0, 0, 0, 0), "expected poll to return TM_BADINIT if no init called first.");
  }