int wsrep_remove_sst_user(bool initialize_thread) {
  int err = 0;
  MYSQL_SESSION session = NULL;

  // Skip the attempt to  mysql.pxc.sst.user in case the server was started with
  // --skip-grant-tables option. It would fail enyway with error.
  // This will prevent writing out error to error log.
  if (skip_grant_tables()) {
    return ECANCELED;
  }
  // This array is filled with pairs of entries
  // The first entry is the actual query to be run
  // The second entry is the string to be displayed if the query fails
  //  (this can be NULL, in which case the actual query will be used)
  const char *cmds[] = {"SET SESSION sql_log_bin = OFF;",
                        nullptr,
                        "SET SESSION lock_wait_timeout = 1;",
                        nullptr,
                        "DROP USER IF EXISTS 'mysql.pxc.sst.user'@localhost;",
                        nullptr,
                        nullptr,
                        nullptr};

  wsrep_allow_server_session = true;
  session = setup_server_session(initialize_thread);
  if (!session) {
    wsrep_allow_server_session = false;
    return ECANCELED;
  }

  for (int index = 0; !err && cmds[index]; index += 2) {
    // Ignore errors, as those are also ignored during user creation
    err = server_session_execute(session, cmds[index], cmds[index + 1], true);
  }

  cleanup_server_session(session, initialize_thread);
  wsrep_allow_server_session = false;
  return err;
}