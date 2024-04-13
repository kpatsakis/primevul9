int MonClient::get_auth_request(
  Connection *con,
  AuthConnectionMeta *auth_meta,
  uint32_t *auth_method,
  std::vector<uint32_t> *preferred_modes,
  bufferlist *bl)
{
  std::lock_guard l(monc_lock);
  ldout(cct,10) << __func__ << " con " << con << " auth_method " << *auth_method
		<< dendl;

  // connection to mon?
  if (con->get_peer_type() == CEPH_ENTITY_TYPE_MON) {
    ceph_assert(!auth_meta->authorizer);
    for (auto& i : pending_cons) {
      if (i.second.is_con(con)) {
	return i.second.get_auth_request(
	  auth_method, preferred_modes, bl,
	  entity_name, want_keys, rotating_secrets.get());
      }
    }
    return -ENOENT;
  }

  // generate authorizer
  if (!auth) {
    lderr(cct) << __func__ << " but no auth handler is set up" << dendl;
    return -EACCES;
  }
  auth_meta->authorizer.reset(auth->build_authorizer(con->get_peer_type()));
  if (!auth_meta->authorizer) {
    lderr(cct) << __func__ << " failed to build_authorizer for type "
	       << ceph_entity_type_name(con->get_peer_type()) << dendl;
    return -EACCES;
  }
  auth_meta->auth_method = auth_meta->authorizer->protocol;
  auth_registry.get_supported_modes(con->get_peer_type(),
				    auth_meta->auth_method,
				    preferred_modes);
  *bl = auth_meta->authorizer->bl;
  return 0;
}