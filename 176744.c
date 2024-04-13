static ssize_t sst_prepare_other(const char *method, const char *addr_in,
                                 const char **addr_out) {
  int const cmd_len = 4096;
  wsp::string cmd_str(cmd_len);

  if (!cmd_str()) {
    WSREP_ERROR(
        "sst_prepare_other(): could not allocate cmd buffer of %d bytes",
        cmd_len);
    return -ENOMEM;
  }

  const char *binlog_opt = "";
  char *binlog_opt_val = NULL;

  int ret;
  if ((ret = generate_binlog_opt_val(&binlog_opt_val))) {
    WSREP_ERROR("sst_prepare_other(): generate_binlog_opt_val() failed: %d",
                ret);
    return ret;
  }
  if (strlen(binlog_opt_val)) binlog_opt = WSREP_SST_OPT_BINLOG;

  ret = snprintf(cmd_str(), cmd_len,
                 "wsrep_sst_%s " WSREP_SST_OPT_ROLE
                 " 'joiner' " WSREP_SST_OPT_ADDR " '%s' " WSREP_SST_OPT_DATA
                 " '%s' " WSREP_SST_OPT_BASEDIR " '%s' " WSREP_SST_OPT_PLUGINDIR
                 " '%s' " WSREP_SST_OPT_CONF " '%s' " WSREP_SST_OPT_CONF_SUFFIX
                 " '%s' " WSREP_SST_OPT_PARENT " '%d' " WSREP_SST_OPT_VERSION
                 " '%s' "
                 " %s '%s' ",
                 method, addr_in, mysql_real_data_home,
                 mysql_home_ptr ? mysql_home_ptr : "",
                 opt_plugin_dir_ptr ? opt_plugin_dir_ptr : "",
                 wsrep_defaults_file, wsrep_defaults_group_suffix,
                 (int)getpid(), MYSQL_SERVER_VERSION MYSQL_SERVER_SUFFIX_DEF,
                 binlog_opt, binlog_opt_val);
  my_free(binlog_opt_val);

  if (ret < 0 || ret >= cmd_len) {
    WSREP_ERROR("sst_prepare_other(): snprintf() failed: %d", ret);
    return (ret < 0 ? ret : -EMSGSIZE);
  }

  wsp::env env(NULL);
  if (env.error()) {
    WSREP_ERROR("sst_prepare_other(): env. var ctor failed: %d", -env.error());
    return -env.error();
  }

  pthread_t tmp;
  sst_thread_arg arg(cmd_str(), env());
  mysql_mutex_lock(&arg.LOCK_wsrep_sst_thread);
  ret = pthread_create(&tmp, NULL, sst_joiner_thread, &arg);
  if (ret) {
    WSREP_ERROR("sst_prepare_other(): pthread_create() failed: %d (%s)", ret,
                strerror(ret));
    return -ret;
  }
  mysql_cond_wait(&arg.COND_wsrep_sst_thread, &arg.LOCK_wsrep_sst_thread);

  *addr_out = arg.ret_str;

  if (!arg.err)
    ret = strlen(*addr_out);
  else {
    assert(arg.err < 0);
    ret = arg.err;
  }

  pthread_detach(tmp);

  return ret;
}