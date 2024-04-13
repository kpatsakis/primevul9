static void *sst_joiner_thread(void *a) {
  sst_thread_arg *arg = (sst_thread_arg *)a;
  int err = 1;

#ifdef HAVE_PSI_INTERFACE
  wsrep_pfs_register_thread(key_THREAD_wsrep_sst_joiner);
#endif /* HAVE_PSI_INTERFACE */

  {
    THD *thd;
    const char magic[] = "ready";
    const size_t magic_len = sizeof(magic) - 1;
    const int out_len = 512;
    char out[out_len];

    WSREP_INFO("Initiating SST/IST transfer on JOINER side (%s)", arg->cmd);

    // Launch the SST script and save pointer to its process:

    if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
    wsp::process proc(arg->cmd, "rw", arg->env);
    sst_process = &proc;
    mysql_mutex_unlock(&LOCK_wsrep_sst);

    pthread_t logger_thd = 0;
    sst_logger_thread_arg logger_arg(proc.err_pipe());
    proc.clear_err_pipe();

    err = start_sst_logger_thread(&logger_arg, &logger_thd);

    if (!err && !proc.error()) {
      // Write out to the stdin pipe any parameters (if needed)

      // Close the pipe, so that the other side gets an EOF
      proc.close_write_pipe();
    }
    if (!err && proc.pipe() && !proc.error()) {
      const char *tmp = my_fgets(out, out_len, proc.pipe());
      if (!tmp || strlen(tmp) < (magic_len + 2) ||
          strncasecmp(tmp, magic, magic_len)) {
        if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
        // Print error message if SST is not cancelled:
        if (!sst_cancelled) {
          // The process has exited, so the logger thread should
          // also have exited
          if (logger_thd) {
            pthread_join(logger_thd, NULL);
            logger_thd = 0;
          }
          // Null-pointer is not valid argument for %s formatting (even
          // though it is supported by many compilers):
          WSREP_ERROR("Failed to read '%s <addr>' from: %s\n\tRead: '%s'",
                      magic, arg->cmd, tmp ? tmp : "(null)");
        }
        // Clear the pointer to SST process:
        sst_process = NULL;
        mysql_mutex_unlock(&LOCK_wsrep_sst);
        proc.wait();
        if (proc.error()) err = proc.error();
      } else {
        // Clear the pointer to SST process:
        if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
        sst_process = NULL;
        mysql_mutex_unlock(&LOCK_wsrep_sst);
        err = 0;
      }
    } else {
      // Clear the pointer to SST process:
      if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
      sst_process = NULL;
      mysql_mutex_unlock(&LOCK_wsrep_sst);
      err = proc.error();
      WSREP_ERROR("Failed to execute: %s : %d (%s)", arg->cmd, err,
                  strerror(err));
    }

    // signal sst_prepare thread with ret code,
    // it will go on sending SST request
    mysql_mutex_lock(&arg->LOCK_wsrep_sst_thread);
    if (!err) {
      arg->ret_str = strdup(out + magic_len + 1);
      if (!arg->ret_str) err = ENOMEM;
    }
    arg->err = -err;
    mysql_cond_signal(&arg->COND_wsrep_sst_thread);
    mysql_mutex_unlock(&arg->LOCK_wsrep_sst_thread);

    if (err) {
      // The process has exited, so the logger thread should
      // also have exited
      if (logger_thd) pthread_join(logger_thd, NULL);

      return NULL; /* lp:808417 - return immediately, don't signal
                    * initializer thread to ensure single thread of
                    * shutdown. */
    }

    wsrep_uuid_t ret_uuid = WSREP_UUID_UNDEFINED;
    wsrep_seqno_t ret_seqno = WSREP_SEQNO_UNDEFINED;

    // in case of successfull receiver start, wait for SST completion/end
    char *tmp = my_fgets(out, out_len, proc.pipe());

    proc.wait();
    err = EINVAL;

    // The process has exited, so the logger thread should
    // also have exited
    if (logger_thd) pthread_join(logger_thd, NULL);

    if (!tmp || proc.error()) {
      WSREP_ERROR("Failed to read uuid:seqno from joiner script.");
      if (proc.error()) {
        err = proc.error();
        char errbuf[MYSYS_STRERROR_SIZE];
        WSREP_ERROR("SST script aborted with error %d (%s)", err,
                    my_strerror(errbuf, sizeof(errbuf), err));
      }
    } else {
      err = sst_scan_uuid_seqno(out, &ret_uuid, &ret_seqno);
    }

    wsrep::gtid ret_gtid;

    if (err) {
      ret_gtid = wsrep::gtid::undefined();
    } else {
      ret_gtid = wsrep::gtid(wsrep::id(ret_uuid.data, sizeof(ret_uuid.data)),
                             wsrep::seqno(ret_seqno));
    }

    // Tell initializer thread that SST is complete
    if (my_thread_init()) {
      WSREP_ERROR(
          "my_thread_init() failed, can't signal end of SST. "
          "Aborting.");
      unireg_abort(1);
    }

    thd = new THD;
    thd->set_new_thread_id();

    if (!thd) {
      WSREP_ERROR(
          "Failed to allocate THD to restore view from local state, "
          "can't signal end of SST. Aborting.");
      unireg_abort(1);
    }

    thd->thread_stack = (char *)&thd;
    thd->security_context()->skip_grants();
    thd->system_thread = SYSTEM_THREAD_BACKGROUND;
    thd->real_id = pthread_self();
    wsrep_assign_from_threadvars(thd);
    wsrep_store_threadvars(thd);

    /* */
    thd->variables.wsrep_on = 0;
    /* No binlogging */
    thd->variables.sql_log_bin = 0;
    thd->variables.option_bits &= ~OPTION_BIN_LOG;
    /* No general log */
    thd->variables.option_bits |= OPTION_LOG_OFF;
    /* Read committed isolation to avoid gap locking */
    thd->variables.transaction_isolation = ISO_READ_COMMITTED;

    wsrep_sst_complete(thd, -err);
    WSREP_SYSTEM("SST completed");
    delete thd;
    my_thread_end();
  }

#ifdef HAVE_PSI_INTERFACE
  wsrep_pfs_delete_thread();
#endif /* HAVE_PSI_INTERFACE */

  return NULL;
}