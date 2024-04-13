static int start_sst_logger_thread(sst_logger_thread_arg *arg, pthread_t *thd) {
  int ret;

  ret = pthread_create(thd, NULL, sst_logger_thread, arg);
  if (ret) {
    *thd = 0;
    WSREP_ERROR("start_sst_logger_thread(): pthread_create() failed: %d (%s)",
                ret, strerror(ret));
    return -ret;
  }
  return 0;
}