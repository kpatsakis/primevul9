void wsrep_sst_cancel(bool call_wsrep_cb) {
  if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
  if (!sst_cancelled) {
    WSREP_INFO("Initiating SST cancellation");
    sst_cancelled = true;
    /*
      When we launched the SST process, then we need
      to terminate it before exit from the parent (server)
      process:
    */
    if (sst_process) {
      WSREP_INFO("Terminating SST process");
      sst_process->terminate();
      sst_process = NULL;
    }
    /*
      If this is a normal shutdown, then we need to notify
      the wsrep provider about completion of the SST, to
      prevent infinite waitng in the wsrep provider after
      the SST process was canceled:
    */
    if (call_wsrep_cb && sst_awaiting_callback) {
      WSREP_INFO("Signalling cancellation of the SST request.");
      const wsrep::gtid state_id = wsrep::gtid::undefined();
      sst_awaiting_callback = false;
      Wsrep_server_state::instance().provider().sst_received(state_id,
                                                             -ECANCELED);
    }
  }
  mysql_mutex_unlock(&LOCK_wsrep_sst);
}