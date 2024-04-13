int MonConnection::handle_auth_bad_method(
  uint32_t old_auth_method,
  int result,
  const std::vector<uint32_t>& allowed_methods,
  const std::vector<uint32_t>& allowed_modes)
{
  ldout(cct,10) << __func__ << " old_auth_method " << old_auth_method
		<< " result " << cpp_strerror(result)
		<< " allowed_methods " << allowed_methods << dendl;
  vector<uint32_t> auth_supported;
  auth_registry->get_supported_methods(con->get_peer_type(), &auth_supported);
  auto p = std::find(auth_supported.begin(), auth_supported.end(),
		     old_auth_method);
  assert(p != auth_supported.end());
  p = std::find_first_of(std::next(p), auth_supported.end(),
			 allowed_methods.begin(), allowed_methods.end());
  if (p == auth_supported.end()) {
    lderr(cct) << __func__ << " server allowed_methods " << allowed_methods
	       << " but i only support " << auth_supported << dendl;
    return -EACCES;
  }
  auth_method = *p;
  ldout(cct,10) << __func__ << " will try " << auth_method << " next" << dendl;
  return 0;
}