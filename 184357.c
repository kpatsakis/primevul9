int MonClient::init()
{
  ldout(cct, 10) << __func__ << dendl;

  entity_name = cct->_conf->name;

  auth_registry.refresh_config();

  std::lock_guard l(monc_lock);
  keyring.reset(new KeyRing);
  if (auth_registry.is_supported_method(messenger->get_mytype(),
					CEPH_AUTH_CEPHX)) {
    // this should succeed, because auth_registry just checked!
    int r = keyring->from_ceph_context(cct);
    if (r != 0) {
      // but be somewhat graceful in case there was a race condition
      lderr(cct) << "keyring not found" << dendl;
      return r;
    }
  }
  if (!auth_registry.any_supported_methods(messenger->get_mytype())) {
    return -ENOENT;
  }

  rotating_secrets.reset(
    new RotatingKeyRing(cct, cct->get_module_type(), keyring.get()));

  initialized = true;

  messenger->set_auth_client(this);
  messenger->add_dispatcher_head(this);

  timer.init();
  finisher.start();
  schedule_tick();

  return 0;
}