int MonClient::handle_auth_bad_method(
  Connection *con,
  AuthConnectionMeta *auth_meta,
  uint32_t old_auth_method,
  int result,
  const std::vector<uint32_t>& allowed_methods,
  const std::vector<uint32_t>& allowed_modes)
{
  auth_meta->allowed_methods = allowed_methods;

  std::lock_guard l(monc_lock);
  if (con->get_peer_type() == CEPH_ENTITY_TYPE_MON) {
    for (auto& i : pending_cons) {
      if (i.second.is_con(con)) {
	int r = i.second.handle_auth_bad_method(old_auth_method,
						result,
						allowed_methods,
						allowed_modes);
	if (r == 0) {
	  return r; // try another method on this con
	}
	pending_cons.erase(i.first);
	if (!pending_cons.empty()) {
	  return r;  // fail this con, maybe another con will succeed
	}
	// fail hunt
	_finish_hunting(r);
	_finish_auth(r);
	return r;
      }
    }
    return -ENOENT;
  } else {
    // huh...
    ldout(cct,10) << __func__ << " hmm, they didn't like " << old_auth_method
		  << " result " << cpp_strerror(result)
		  << " and auth is " << (auth ? auth->get_protocol() : 0)
		  << dendl;
    return -EACCES;
  }
}