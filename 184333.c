void MonClient::_finish_hunting(int auth_err)
{
  ldout(cct,10) << __func__ << " " << auth_err << dendl;
  ceph_assert(monc_lock.is_locked());
  // the pending conns have been cleaned.
  ceph_assert(!_hunting());
  if (active_con) {
    auto con = active_con->get_con();
    ldout(cct, 1) << "found mon."
		  << monmap.get_name(con->get_peer_addr())
		  << dendl;
  } else {
    ldout(cct, 1) << "no mon sessions established" << dendl;
  }

  had_a_connection = true;
  _un_backoff();

  if (!auth_err) {
    last_rotating_renew_sent = utime_t();
    while (!waiting_for_session.empty()) {
      _send_mon_message(waiting_for_session.front());
      waiting_for_session.pop_front();
    }
    _resend_mon_commands();
    send_log(true);
    if (active_con) {
      std::swap(auth, active_con->get_auth());
      if (global_id && global_id != active_con->get_global_id()) {
	lderr(cct) << __func__ << " global_id changed from " << global_id
		   << " to " << active_con->get_global_id() << dendl;
      }
      global_id = active_con->get_global_id();
    }
  }
}