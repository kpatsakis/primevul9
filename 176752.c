static int sst_donate_other(const char *method, const char *addr,
                            const wsrep::gtid &gtid, bool bypass,
                            char **env)  // carries auth info
{
  int const cmd_len = 4096;
  wsp::string cmd_str(cmd_len);

  if (!cmd_str()) {
    WSREP_ERROR(
        "sst_donate_other(): "
        "could not allocate cmd buffer of %d bytes",
        cmd_len);
    return -ENOMEM;
  }

  const char *binlog_opt = "";
  char *binlog_opt_val = NULL;

  int ret;
  if ((ret = generate_binlog_opt_val(&binlog_opt_val))) {
    WSREP_ERROR("sst_donate_other(): generate_binlog_opt_val() failed: %d",
                ret);
    return ret;
  }
  if (strlen(binlog_opt_val)) binlog_opt = WSREP_SST_OPT_BINLOG;

  std::ostringstream uuid_oss;
  uuid_oss << gtid.id();

  ret = snprintf(
      cmd_str(), cmd_len,
      "wsrep_sst_%s " WSREP_SST_OPT_ROLE " 'donor' " WSREP_SST_OPT_ADDR
      " '%s' " WSREP_SST_OPT_SOCKET " '%s' " WSREP_SST_OPT_DATA
      " '%s' " WSREP_SST_OPT_BASEDIR " '%s' " WSREP_SST_OPT_PLUGINDIR
      " '%s' " WSREP_SST_OPT_CONF " '%s' " WSREP_SST_OPT_CONF_SUFFIX
      " '%s' " WSREP_SST_OPT_VERSION
      " '%s' "
      " %s '%s' " WSREP_SST_OPT_GTID
      " '%s:%lld' "
      "%s",
      method, addr, mysqld_unix_port, mysql_real_data_home,
      mysql_home_ptr ? mysql_home_ptr : "",
      opt_plugin_dir_ptr ? opt_plugin_dir_ptr : "", wsrep_defaults_file,
      wsrep_defaults_group_suffix, MYSQL_SERVER_VERSION MYSQL_SERVER_SUFFIX_DEF,
      binlog_opt, binlog_opt_val, uuid_oss.str().c_str(), gtid.seqno().get(),
      bypass ? " " WSREP_SST_OPT_BYPASS : "");
  my_free(binlog_opt_val);

  if (ret < 0 || ret >= cmd_len) {
    WSREP_ERROR("sst_donate_other(): snprintf() failed: %d", ret);
    return (ret < 0 ? ret : -EMSGSIZE);
  }

  if (!bypass && wsrep_sst_donor_rejects_queries) sst_reject_queries(false);

  pthread_t tmp;
  sst_thread_arg arg(cmd_str(), env);
  mysql_mutex_lock(&arg.LOCK_wsrep_sst_thread);
  ret = pthread_create(&tmp, NULL, sst_donor_thread, &arg);
  if (ret) {
    WSREP_ERROR("sst_donate_other(): pthread_create() failed: %d (%s)", ret,
                strerror(ret));
    return ret;
  }
  mysql_cond_wait(&arg.COND_wsrep_sst_thread, &arg.LOCK_wsrep_sst_thread);

  WSREP_INFO("DONOR thread signaled with %d", arg.err);
  pthread_detach(tmp);
  return arg.err;
}