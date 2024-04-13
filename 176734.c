static int wsrep_create_sst_user(bool initialize_thread, const char *password) {
  int err = 0;
  int const auth_len = 512;
  char auth_buf[auth_len];
  MYSQL_SESSION session = NULL;

  // This array is filled with pairs of entries
  // The first entry is the actual query to be run
  // The second entry is the string to be displayed if the query fails
  //  (this can be NULL, in which case the actual query will be used)
  const char *cmds[] = {
    "SET SESSION sql_log_bin = OFF;",
    nullptr,
    "DROP USER IF EXISTS 'mysql.pxc.sst.user'@localhost;",
    nullptr,
    "CREATE USER 'mysql.pxc.sst.user'@localhost "
    " IDENTIFIED BY '%s' ACCOUNT LOCK;",
    "CREATE USER mysql.pxc.sst.user IDENTIFIED WITH * BY * ACCOUNT LOCK",
  /*
    This is the code that uses the mysql.pxc.sst.role
    However there is a bug in 8.0.15 where the "GRANT CREATE ON DBNAME.*" when
    used in a role, does not allow the user with the role to create a database.
    So we have to explicitly grant the privileges.
  */
#if 0
    "GRANT 'mysql.pxc.sst.role'@localhost TO 'mysql.pxc.sst.user'@localhost;", nullptr,
    "SET DEFAULT ROLE 'mysql.pxc.sst.role'@localhost to 'mysql.pxc.sst.user'@localhost;", nullptr,
#else
    /*
      Explicit privileges needed to run XtraBackup.  This is only used due
      to the bug in 8.0.15 described above.
    */
    "GRANT BACKUP_ADMIN, LOCK TABLES, PROCESS, RELOAD, REPLICATION CLIENT, "
    "SUPER ON *.* TO 'mysql.pxc.sst.user'@localhost;",
    nullptr,
    "GRANT CREATE, INSERT, SELECT ON PERCONA_SCHEMA.xtrabackup_history TO "
    "'mysql.pxc.sst.user'@localhost;",
    nullptr,
    "GRANT SELECT ON performance_schema.* TO 'mysql.pxc.sst.user'@localhost;",
    nullptr,
    "GRANT CREATE USER ON *.* to 'mysql.pxc.sst.user'@localhost;",
    nullptr,
    "GRANT CREATE ON PERCONA_SCHEMA.* to 'mysql.pxc.sst.user'@localhost;",
    nullptr,
#endif

    "ALTER USER 'mysql.pxc.sst.user'@localhost ACCOUNT UNLOCK;",
    nullptr,
    nullptr,
    nullptr
  };

  wsrep_allow_server_session = true;
  session = setup_server_session(initialize_thread);
  if (!session) {
    wsrep_allow_server_session = false;
    return ECANCELED;
  }

  for (int index = 0; !err && cmds[index]; index += 2) {
    int ret;
    ret = snprintf(auth_buf, auth_len, cmds[index], password);
    if (ret < 0 || ret >= auth_len) {
      WSREP_ERROR("wsrep_create_sst_user() : snprintf() failed: %d", ret);
      err = (ret < 0 ? ret : -EMSGSIZE);
      break;
    }
    // Ignore errors: also ignored in the boostrap code
    err = server_session_execute(session, auth_buf, cmds[index + 1], true);
  }

  // Overwrite query (clear out any sensitive data)
  ::memset(auth_buf, 0, auth_len);

  cleanup_server_session(session, initialize_thread);
  wsrep_allow_server_session = false;
  return err;
}