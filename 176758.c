static int sst_run_shell(const char *cmd_str, char **env, int max_tries) {
  int ret = 0;

  for (int tries = 1; tries <= max_tries; tries++) {
    // Launch the SST script and save pointer to its process:

    if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
    wsp::process proc(cmd_str, "r", env);
    sst_process = &proc;
    mysql_mutex_unlock(&LOCK_wsrep_sst);

    if (proc.pipe() && !proc.error()) {
      proc.wait();
    }

    // Clear the pointer to SST process:

    if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
    sst_process = NULL;
    mysql_mutex_unlock(&LOCK_wsrep_sst);

    if ((ret = proc.error())) {
      // Try again if SST is not cancelled:
      if (mysql_mutex_lock(&LOCK_wsrep_sst)) abort();
      if (!sst_cancelled) {
        mysql_mutex_unlock(&LOCK_wsrep_sst);
        WSREP_ERROR("Try %d/%d: '%s' failed: %d (%s)", tries, max_tries,
                    proc.cmd(), ret, strerror(ret));
        sleep(1);
      } else {
        mysql_mutex_unlock(&LOCK_wsrep_sst);
      }
    } else {
      WSREP_DEBUG("SST script successfully completed.");
      break;
    }
  }

  return -ret;
}