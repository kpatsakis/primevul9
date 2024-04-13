int MonClient::get_monmap_and_config()
{
  ldout(cct, 10) << __func__ << dendl;
  ceph_assert(!messenger);

  int tries = 10;

  utime_t interval;
  interval.set_from_double(cct->_conf->mon_client_hunt_interval);

  cct->init_crypto();
  auto shutdown_crypto = make_scope_guard([this] {
    cct->shutdown_crypto();
  });

  int r = build_initial_monmap();
  if (r < 0) {
    lderr(cct) << __func__ << " cannot identify monitors to contact" << dendl;
    return r;
  }

  messenger = Messenger::create_client_messenger(
    cct, "temp_mon_client");
  ceph_assert(messenger);
  messenger->add_dispatcher_head(this);
  messenger->start();
  auto shutdown_msgr = make_scope_guard([this] {
    messenger->shutdown();
    messenger->wait();
    delete messenger;
    messenger = nullptr;
    if (!monmap.fsid.is_zero()) {
      cct->_conf.set_val("fsid", stringify(monmap.fsid));
    }
  });

  while (tries-- > 0) {
    r = init();
    if (r < 0) {
      return r;
    }
    r = authenticate(cct->_conf->client_mount_timeout);
    if (r == -ETIMEDOUT) {
      shutdown();
      continue;
    }
    if (r < 0) {
      break;
    }
    {
      std::lock_guard l(monc_lock);
      if (monmap.get_epoch() &&
	  !monmap.persistent_features.contains_all(
	    ceph::features::mon::FEATURE_MIMIC)) {
	ldout(cct,10) << __func__ << " pre-mimic monitor, no config to fetch"
		      << dendl;
	r = 0;
	break;
      }
      while ((!got_config || monmap.get_epoch() == 0) && r == 0) {
	ldout(cct,20) << __func__ << " waiting for monmap|config" << dendl;
	r = map_cond.WaitInterval(monc_lock, interval);
      }
      if (got_config) {
	ldout(cct,10) << __func__ << " success" << dendl;
	r = 0;
	break;
      }
    }
    lderr(cct) << __func__ << " failed to get config" << dendl;
    shutdown();
    continue;
  }

  shutdown();
  return r;
}