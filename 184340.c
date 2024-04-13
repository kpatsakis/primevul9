void MonConnection::start(epoch_t epoch,
			  const EntityName& entity_name)
{
  auth_start = ceph_clock_now();

  if (con->get_peer_addr().is_msgr2()) {
    ldout(cct, 10) << __func__ << " opening mon connection" << dendl;
    state = State::AUTHENTICATING;
    con->send_message(new MMonGetMap());
    return;
  }

  // restart authentication handshake
  state = State::NEGOTIATING;

  // send an initial keepalive to ensure our timestamp is valid by the
  // time we are in an OPENED state (by sequencing this before
  // authentication).
  con->send_keepalive();

  auto m = new MAuth;
  m->protocol = CEPH_AUTH_UNKNOWN;
  m->monmap_epoch = epoch;
  __u8 struct_v = 1;
  encode(struct_v, m->auth_payload);
  vector<uint32_t> auth_supported;
  auth_registry->get_supported_methods(con->get_peer_type(), &auth_supported);
  encode(auth_supported, m->auth_payload);
  encode(entity_name, m->auth_payload);
  encode(global_id, m->auth_payload);
  con->send_message(m);
}