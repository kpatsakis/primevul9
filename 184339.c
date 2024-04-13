MonClient::MonClient(CephContext *cct_) :
  Dispatcher(cct_),
  AuthServer(cct_),
  messenger(NULL),
  monc_lock("MonClient::monc_lock"),
  timer(cct_, monc_lock),
  finisher(cct_),
  initialized(false),
  log_client(NULL),
  more_log_pending(false),
  want_monmap(true),
  had_a_connection(false),
  reopen_interval_multiplier(
    cct_->_conf.get_val<double>("mon_client_hunt_interval_min_multiple")),
  last_mon_command_tid(0),
  version_req_id(0)
{}