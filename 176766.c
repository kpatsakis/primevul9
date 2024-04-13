static int sst_flush_tables(THD *thd) {
  WSREP_INFO("Flushing tables for SST...");

  int err;
  int not_used;
  err = run_sql_command(thd, "FLUSH TABLES WITH READ LOCK", NULL);
  if (err) {
    if (err == ER_UNKNOWN_SYSTEM_VARIABLE)
      WSREP_WARN("Was mysqld built with --with-innodb-disallow-writes ?");
    WSREP_ERROR("Failed to flush and lock tables");
    err = ECANCELED;
  } else {
    /* make sure logs are flushed after global read lock acquired */
    err = handle_reload_request(thd, REFRESH_ENGINE_LOG | REFRESH_BINARY_LOG,
                                (TABLE_LIST *)0, &not_used);
  }

  if (err) {
    WSREP_ERROR("Failed to flush tables: %d (%s)", err, strerror(err));
  } else {
    WSREP_INFO("Table flushing completed.");
    const char base_name[] = "tables_flushed";
    ssize_t const full_len =
        strlen(mysql_real_data_home) + strlen(base_name) + 2;
    char *real_name = static_cast<char *>(alloca(full_len));
    sprintf(real_name, "%s/%s", mysql_real_data_home, base_name);
    char *tmp_name = static_cast<char *>(alloca(full_len + 4));
    sprintf(tmp_name, "%s.tmp", real_name);

    FILE *file = fopen(tmp_name, "w+");
    if (0 == file) {
      err = errno;
      WSREP_ERROR("Failed to open '%s': %d (%s)", tmp_name, err, strerror(err));
    } else {
      Wsrep_server_state &server_state = Wsrep_server_state::instance();
      std::ostringstream uuid_oss;

      uuid_oss << server_state.current_view().state_id().id();

      fprintf(file, "%s:%lld\n", uuid_oss.str().c_str(),
              server_state.pause_seqno().get());
      fsync(fileno(file));
      fclose(file);
      if (rename(tmp_name, real_name) == -1) {
        err = errno;
        WSREP_ERROR("Failed to rename '%s' to '%s': %d (%s)", tmp_name,
                    real_name, err, strerror(err));
      }
    }
  }

  return err;
}