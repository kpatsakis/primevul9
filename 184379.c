int MonClient::ping_monitor(const string &mon_id, string *result_reply)
{
  ldout(cct, 10) << __func__ << dendl;

  string new_mon_id;
  if (monmap.contains("noname-"+mon_id)) {
    new_mon_id = "noname-"+mon_id;
  } else {
    new_mon_id = mon_id;
  }

  if (new_mon_id.empty()) {
    ldout(cct, 10) << __func__ << " specified mon id is empty!" << dendl;
    return -EINVAL;
  } else if (!monmap.contains(new_mon_id)) {
    ldout(cct, 10) << __func__ << " no such monitor 'mon." << new_mon_id << "'"
                   << dendl;
    return -ENOENT;
  }

  // N.B. monc isn't initialized

  auth_registry.refresh_config();

  KeyRing keyring;
  keyring.from_ceph_context(cct);
  RotatingKeyRing rkeyring(cct, cct->get_module_type(), &keyring);

  MonClientPinger *pinger = new MonClientPinger(cct,
						&rkeyring,
						result_reply);

  Messenger *smsgr = Messenger::create_client_messenger(cct, "temp_ping_client");
  smsgr->add_dispatcher_head(pinger);
  smsgr->set_auth_client(pinger);
  smsgr->start();

  ConnectionRef con = smsgr->connect_to_mon(monmap.get_addrs(new_mon_id));
  ldout(cct, 10) << __func__ << " ping mon." << new_mon_id
                 << " " << con->get_peer_addr() << dendl;

  pinger->mc.reset(new MonConnection(cct, con, 0, &auth_registry));
  pinger->mc->start(monmap.get_epoch(), entity_name);
  con->send_message(new MPing);

  pinger->lock.Lock();
  int ret = pinger->wait_for_reply(cct->_conf->mon_client_ping_timeout);
  if (ret == 0) {
    ldout(cct,10) << __func__ << " got ping reply" << dendl;
  } else {
    ret = -ret;
  }
  pinger->lock.Unlock();

  con->mark_down();
  pinger->mc.reset();
  smsgr->shutdown();
  smsgr->wait();
  delete smsgr;
  delete pinger;
  return ret;
}