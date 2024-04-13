AuthAuthorizer* MonClient::build_authorizer(int service_id) const {
  std::lock_guard l(monc_lock);
  if (auth) {
    return auth->build_authorizer(service_id);
  } else {
    ldout(cct, 0) << __func__ << " for " << ceph_entity_type_name(service_id)
		  << ", but no auth is available now" << dendl;
    return nullptr;
  }
}