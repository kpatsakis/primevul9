  sst_thread_arg(const char *c, char **e)
      : cmd(c), env(e), ret_str(0), err(-1) {
    mysql_mutex_init(key_LOCK_wsrep_sst_thread, &LOCK_wsrep_sst_thread,
                     MY_MUTEX_INIT_FAST);
    mysql_cond_init(key_COND_wsrep_sst_thread, &COND_wsrep_sst_thread);
  }