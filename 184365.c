void MonClient::_send_command(MonCommand *r)
{
  ++r->send_attempts;

  entity_addr_t peer;
  if (active_con) {
    peer = active_con->get_con()->get_peer_addr();
  }

  if (r->target_rank >= 0 &&
      r->target_rank != monmap.get_rank(peer)) {
    if (r->send_attempts > cct->_conf->mon_client_directed_command_retry) {
      _finish_command(r, -ENXIO, "mon unavailable");
      return;
    }
    ldout(cct, 10) << __func__ << " " << r->tid << " " << r->cmd
		   << " wants rank " << r->target_rank
		   << ", reopening session"
		   << dendl;
    if (r->target_rank >= (int)monmap.size()) {
      ldout(cct, 10) << " target " << r->target_rank << " >= max mon " << monmap.size() << dendl;
      _finish_command(r, -ENOENT, "mon rank dne");
      return;
    }
    _reopen_session(r->target_rank);
    return;
  }

  if (r->target_name.length() &&
      r->target_name != monmap.get_name(peer)) {
    if (r->send_attempts > cct->_conf->mon_client_directed_command_retry) {
      _finish_command(r, -ENXIO, "mon unavailable");
      return;
    }
    ldout(cct, 10) << __func__ << " " << r->tid << " " << r->cmd
		   << " wants mon " << r->target_name
		   << ", reopening session"
		   << dendl;
    if (!monmap.contains(r->target_name)) {
      ldout(cct, 10) << " target " << r->target_name << " not present in monmap" << dendl;
      _finish_command(r, -ENOENT, "mon dne");
      return;
    }
    _reopen_session(monmap.get_rank(r->target_name));
    return;
  }

  ldout(cct, 10) << __func__ << " " << r->tid << " " << r->cmd << dendl;
  MMonCommand *m = new MMonCommand(monmap.fsid);
  m->set_tid(r->tid);
  m->cmd = r->cmd;
  m->set_data(r->inbl);
  _send_mon_message(m);
  return;
}