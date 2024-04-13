int MonConnection::_negotiate(MAuthReply *m,
			      const EntityName& entity_name,
			      uint32_t want_keys,
			      RotatingKeyRing* keyring)
{
  if (auth && (int)m->protocol == auth->get_protocol()) {
    // good, negotiation completed
    auth->reset();
    return 0;
  }

  int r = _init_auth(m->protocol, entity_name, want_keys, keyring, false);
  if (r == -ENOTSUP) {
    if (m->result == -ENOTSUP) {
      ldout(cct, 10) << "none of our auth protocols are supported by the server"
		     << dendl;
    }
    return m->result;
  }
  return r;
}