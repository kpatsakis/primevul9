void MonClient::_send_mon_message(Message *m)
{
  ceph_assert(monc_lock.is_locked());
  if (active_con) {
    auto cur_con = active_con->get_con();
    ldout(cct, 10) << "_send_mon_message to mon."
		   << monmap.get_name(cur_con->get_peer_addr())
		   << " at " << cur_con->get_peer_addr() << dendl;
    cur_con->send_message(m);
  } else {
    waiting_for_session.push_back(m);
  }
}