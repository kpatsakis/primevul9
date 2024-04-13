static void *sst_donor_thread(void *a) {
  sst_thread_arg *arg = (sst_thread_arg *)a;

#ifdef HAVE_PSI_INTERFACE
  wsrep_pfs_register_thread(key_THREAD_wsrep_sst_donor);
#endif /* HAVE_PSI_INTERFACE */

  WSREP_INFO("Initiating SST/IST transfer on DONOR side (%s)", arg->cmd);

  int err = 1;
  bool locked = false;

  const char *out = NULL;
  int const out_len = 1024;
  char out_buf[out_len];

  // Generate the random password
  std::string password;
  generate_password(&password, 32);

  wsrep_uuid_t ret_uuid = WSREP_UUID_UNDEFINED;
  wsrep_seqno_t ret_seqno = WSREP_SEQNO_UNDEFINED;  // seqno of complete SST

  wsp::thd thd(false);  // we turn off wsrep_on for this THD so that it can
                        // operate with wsrep_ready == OFF

  // Create the SST auth user
  err = wsrep_create_sst_user(true, password.c_str());

  if (err) {
#ifdef HAVE_PSI_INTERFACE
    wsrep_pfs_delete_thread();
#endif /* HAVE_PSI_INTERFACE */

    /* Inform server about SST script startup and release TO isolation */
    mysql_mutex_lock(&arg->LOCK_wsrep_sst_thread);
    arg->err = -err;
    mysql_cond_signal(&arg->COND_wsrep_sst_thread);
    mysql_mutex_unlock(
        &arg->LOCK_wsrep_sst_thread);  //! @note arg is unusable after that.

    return NULL;
  }

  // Launch the SST script and save pointer to its process:

  if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
  wsp::process proc(arg->cmd, "rw", arg->env, !err /* execute_immediately */);

  pthread_t logger_thd = 0;
  sst_logger_thread_arg logger_arg(proc.err_pipe());
  proc.clear_err_pipe();
  err = start_sst_logger_thread(&logger_arg, &logger_thd);

  if (!err) {
    int ret;
    ret = fprintf(proc.write_pipe(),
                  "sst_user=mysql.pxc.sst.user\n"
                  "sst_password=%s\n",
                  password.c_str());
    if (ret < 0) {
      WSREP_ERROR("sst_donor_thread(): fprintf() failed: %d", ret);
      err = (ret < 0 ? ret : -EMSGSIZE);
    }

    // Close the pipe, so that the other side gets an EOF
    proc.close_write_pipe();
  }
  password.assign(password.length(), 0);  // overwrite password value

  sst_process = &proc;
  mysql_mutex_unlock(&LOCK_wsrep_sst);

  if (!err) err = proc.error();

  /* Inform server about SST script startup and release TO isolation */
  mysql_mutex_lock(&arg->LOCK_wsrep_sst_thread);
  arg->err = -err;
  mysql_cond_signal(&arg->COND_wsrep_sst_thread);
  mysql_mutex_unlock(
      &arg->LOCK_wsrep_sst_thread);  //! @note arg is unusable after that.

  if (proc.pipe() && !err) {
  wait_signal:
    out = my_fgets(out_buf, out_len, proc.pipe());

    if (out) {
      const char magic_flush[] = "flush tables";
      const char magic_cont[] = "continue";
      const char magic_done[] = "done";

      if (!strcasecmp(out, magic_flush)) {
        err = sst_flush_tables(thd.ptr);
        if (!err) {
          sst_disallow_writes(thd.ptr, true);
          locked = true;
          goto wait_signal;
        }
      } else if (!strcasecmp(out, magic_cont)) {
        if (locked) {
          sst_disallow_writes(thd.ptr, false);
          thd.ptr->global_read_lock.unlock_global_read_lock(thd.ptr);
          locked = false;
        }
        err = 0;
        goto wait_signal;
      } else if (!strncasecmp(out, magic_done, strlen(magic_done))) {
        err = sst_scan_uuid_seqno(out + strlen(magic_done) + 1, &ret_uuid,
                                  &ret_seqno);
      } else {
        WSREP_WARN("Received unknown signal: '%s'", out);
      }
    } else {
      if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
        // Print error message if SST is not cancelled:
#if 0
      if (!sst_cancelled)
      {
         WSREP_ERROR("Failed to read from: %s", proc.cmd());
      }
#endif
      // Clear the pointer to SST process:
      sst_process = NULL;
      mysql_mutex_unlock(&LOCK_wsrep_sst);
      proc.wait();
      goto skip_clear_pointer;
    }

    // Clear the pointer to SST process:
    if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
    sst_process = NULL;
    mysql_mutex_unlock(&LOCK_wsrep_sst);

  skip_clear_pointer:
    if (!err && proc.error()) err = proc.error();
  } else {
    // Clear the pointer to SST process:
    if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
    sst_process = NULL;
    mysql_mutex_unlock(&LOCK_wsrep_sst);
    WSREP_ERROR("Failed to execute: %s : %d (%s)", proc.cmd(), err,
                strerror(err));
  }

  wsrep_remove_sst_user(true);

  if (locked)  // don't forget to unlock server before return
  {
    sst_disallow_writes(thd.ptr, false);
    thd.ptr->global_read_lock.unlock_global_read_lock(thd.ptr);
  }

  // signal to donor that SST is over
  wsrep::gtid gtid(
      wsrep::id(ret_uuid.data, sizeof(ret_uuid.data)),
      wsrep::seqno(err ? wsrep::seqno::undefined() : wsrep::seqno(ret_seqno)));
  Wsrep_server_state::instance().sst_sent(gtid, -err);
  proc.wait();

  // The process has exited, so the logger thread should
  // also have exited
  if (logger_thd) pthread_join(logger_thd, NULL);

#ifdef HAVE_PSI_INTERFACE
  wsrep_pfs_delete_thread();
#endif /* HAVE_PSI_INTERFACE */

  return NULL;
}