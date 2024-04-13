int MonConnection::handle_auth(MAuthReply* m,
			       const EntityName& entity_name,
			       uint32_t want_keys,
			       RotatingKeyRing* keyring)
{
  if (state == State::NEGOTIATING) {
    int r = _negotiate(m, entity_name, want_keys, keyring);
    if (r) {
      return r;
    }
    state = State::AUTHENTICATING;
  }
  int r = authenticate(m);
  if (!r) {
    state = State::HAVE_SESSION;
  }
  return r;
}