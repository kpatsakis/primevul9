void MonClient::handle_config(MConfig *m)
{
  ldout(cct,10) << __func__ << " " << *m << dendl;
  finisher.queue(new FunctionContext([this, m](int r) {
	cct->_conf.set_mon_vals(cct, m->config, config_cb);
	if (config_notify_cb) {
	  config_notify_cb();
	}
	m->put();
      }));
  got_config = true;
  map_cond.Signal();
}