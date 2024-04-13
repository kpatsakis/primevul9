void MonClient::tick()
{
  ldout(cct, 10) << __func__ << dendl;

  auto reschedule_tick = make_scope_guard([this] {
      schedule_tick();
    });

  _check_auth_tickets();
  
  if (_hunting()) {
    ldout(cct, 1) << "continuing hunt" << dendl;
    return _reopen_session();
  } else if (active_con) {
    // just renew as needed
    utime_t now = ceph_clock_now();
    auto cur_con = active_con->get_con();
    if (!cur_con->has_feature(CEPH_FEATURE_MON_STATEFUL_SUB)) {
      const bool maybe_renew = sub.need_renew();
      ldout(cct, 10) << "renew subs? -- " << (maybe_renew ? "yes" : "no")
		     << dendl;
      if (maybe_renew) {
	_renew_subs();
      }
    }

    cur_con->send_keepalive();

    if (cct->_conf->mon_client_ping_timeout > 0 &&
	cur_con->has_feature(CEPH_FEATURE_MSGR_KEEPALIVE2)) {
      utime_t lk = cur_con->get_last_keepalive_ack();
      utime_t interval = now - lk;
      if (interval > cct->_conf->mon_client_ping_timeout) {
	ldout(cct, 1) << "no keepalive since " << lk << " (" << interval
		      << " seconds), reconnecting" << dendl;
	return _reopen_session();
      }
      send_log();
    }

    _un_backoff();
  }
}