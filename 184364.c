int MonConnection::_init_auth(
  uint32_t method,
  const EntityName& entity_name,
  uint32_t want_keys,
  RotatingKeyRing* keyring,
  bool msgr2)
{
  ldout(cct,10) << __func__ << " method " << method << dendl;
  auth.reset(
    AuthClientHandler::create(cct, method, keyring));
  if (!auth) {
    ldout(cct, 10) << " no handler for protocol " << method << dendl;
    return -ENOTSUP;
  }

  // do not request MGR key unless the mon has the SERVER_KRAKEN
  // feature.  otherwise it will give us an auth error.  note that
  // we have to use the FEATUREMASK because pre-jewel the kraken
  // feature bit was used for something else.
  if (!msgr2 &&
      (want_keys & CEPH_ENTITY_TYPE_MGR) &&
      !(con->has_features(CEPH_FEATUREMASK_SERVER_KRAKEN))) {
    ldout(cct, 1) << __func__
		  << " not requesting MGR keys from pre-kraken monitor"
		  << dendl;
    want_keys &= ~CEPH_ENTITY_TYPE_MGR;
  }
  auth->set_want_keys(want_keys);
  auth->init(entity_name);
  auth->set_global_id(global_id);
  return 0;
}