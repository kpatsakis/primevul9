int MonConnection::handle_auth_done(
  AuthConnectionMeta *auth_meta,
  uint64_t new_global_id,
  const bufferlist& bl,
  CryptoKey *session_key,
  std::string *connection_secret)
{
  ldout(cct,10) << __func__ << " global_id " << new_global_id
		<< " payload " << bl.length()
		<< dendl;
  global_id = new_global_id;
  auth->set_global_id(global_id);
  auto p = bl.begin();
  int auth_err = auth->handle_response(0, p, &auth_meta->session_key,
				       &auth_meta->connection_secret);
  if (auth_err >= 0) {
    state = State::HAVE_SESSION;
  }
  con->set_last_keepalive_ack(auth_start);
  return auth_err;
}