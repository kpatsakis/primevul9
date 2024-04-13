int MonClient::handle_auth_done(
  Connection *con,
  AuthConnectionMeta *auth_meta,
  uint64_t global_id,
  uint32_t con_mode,
  const bufferlist& bl,
  CryptoKey *session_key,
  std::string *connection_secret)
{
  if (con->get_peer_type() == CEPH_ENTITY_TYPE_MON) {
    std::lock_guard l(monc_lock);
    for (auto& i : pending_cons) {
      if (i.second.is_con(con)) {
	int r = i.second.handle_auth_done(
	  auth_meta, global_id, bl,
	  session_key, connection_secret);
	if (r) {
	  pending_cons.erase(i.first);
	  if (!pending_cons.empty()) {
	    return r;
	  }
	} else {
	  active_con.reset(new MonConnection(std::move(i.second)));
	  pending_cons.clear();
	  ceph_assert(active_con->have_session());
	}

	_finish_hunting(r);
	if (r || monmap.get_epoch() > 0) {
	  _finish_auth(r);
	}
	return r;
      }
    }
    return -ENOENT;
  } else {
    // verify authorizer reply
    auto p = bl.begin();
    if (!auth_meta->authorizer->verify_reply(p, &auth_meta->connection_secret)) {
      ldout(cct, 0) << __func__ << " failed verifying authorizer reply"
		    << dendl;
      return -EACCES;
    }
    auth_meta->session_key = auth_meta->authorizer->session_key;
    return 0;
  }
}