  ~sst_thread_arg() {
    mysql_cond_destroy(&COND_wsrep_sst_thread);
    mysql_mutex_unlock(&LOCK_wsrep_sst_thread);
    mysql_mutex_destroy(&LOCK_wsrep_sst_thread);
  }