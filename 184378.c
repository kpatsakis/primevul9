int MonClient::_check_auth_tickets()
{
  ceph_assert(monc_lock.is_locked());
  if (active_con && auth) {
    if (auth->need_tickets()) {
      ldout(cct, 10) << __func__ << " getting new tickets!" << dendl;
      MAuth *m = new MAuth;
      m->protocol = auth->get_protocol();
      auth->prepare_build_request();
      auth->build_request(m->auth_payload);
      _send_mon_message(m);
    }

    _check_auth_rotating();
  }
  return 0;
}