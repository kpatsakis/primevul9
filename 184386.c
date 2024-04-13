void MonClient::handle_auth(MAuthReply *m)
{
  ceph_assert(monc_lock.is_locked());
  if (!_hunting()) {
    std::swap(active_con->get_auth(), auth);
    int ret = active_con->authenticate(m);
    m->put();
    std::swap(auth, active_con->get_auth());
    if (global_id != active_con->get_global_id()) {
      lderr(cct) << __func__ << " peer assigned me a different global_id: "
		 << active_con->get_global_id() << dendl;
    }
    if (ret != -EAGAIN) {
      _finish_auth(ret);
    }
    return;
  }

  // hunting
  auto found = _find_pending_con(m->get_connection());
  ceph_assert(found != pending_cons.end());
  int auth_err = found->second.handle_auth(m, entity_name, want_keys,
					   rotating_secrets.get());
  m->put();
  if (auth_err == -EAGAIN) {
    return;
  }
  if (auth_err) {
    pending_cons.erase(found);
    if (!pending_cons.empty()) {
      // keep trying with pending connections
      return;
    }
    // the last try just failed, give up.
  } else {
    auto& mc = found->second;
    ceph_assert(mc.have_session());
    active_con.reset(new MonConnection(std::move(mc)));
    pending_cons.clear();
  }

  _finish_hunting(auth_err);
  _finish_auth(auth_err);
}