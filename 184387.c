bool MonClient::ms_handle_reset(Connection *con)
{
  std::lock_guard lock(monc_lock);

  if (con->get_peer_type() != CEPH_ENTITY_TYPE_MON)
    return false;

  if (_hunting()) {
    if (pending_cons.count(con->get_peer_addrs())) {
      ldout(cct, 10) << __func__ << " hunted mon " << con->get_peer_addrs()
		     << dendl;
    } else {
      ldout(cct, 10) << __func__ << " stray mon " << con->get_peer_addrs()
		     << dendl;
    }
    return true;
  } else {
    if (active_con && con == active_con->get_con()) {
      ldout(cct, 10) << __func__ << " current mon " << con->get_peer_addrs()
		     << dendl;
      _reopen_session();
      return false;
    } else {
      ldout(cct, 10) << "ms_handle_reset stray mon " << con->get_peer_addrs()
		     << dendl;
      return true;
    }
  }
}