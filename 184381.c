int MonClient::_check_auth_rotating()
{
  ceph_assert(monc_lock.is_locked());
  if (!rotating_secrets ||
      !auth_principal_needs_rotating_keys(entity_name)) {
    ldout(cct, 20) << "_check_auth_rotating not needed by " << entity_name << dendl;
    return 0;
  }

  if (!active_con || !auth) {
    ldout(cct, 10) << "_check_auth_rotating waiting for auth session" << dendl;
    return 0;
  }

  utime_t now = ceph_clock_now();
  utime_t cutoff = now;
  cutoff -= std::min(30.0, cct->_conf->auth_service_ticket_ttl / 4.0);
  utime_t issued_at_lower_bound = now;
  issued_at_lower_bound -= cct->_conf->auth_service_ticket_ttl;
  if (!rotating_secrets->need_new_secrets(cutoff)) {
    ldout(cct, 10) << "_check_auth_rotating have uptodate secrets (they expire after " << cutoff << ")" << dendl;
    rotating_secrets->dump_rotating();
    return 0;
  }

  ldout(cct, 10) << "_check_auth_rotating renewing rotating keys (they expired before " << cutoff << ")" << dendl;
  if (!rotating_secrets->need_new_secrets() &&
      rotating_secrets->need_new_secrets(issued_at_lower_bound)) {
    // the key has expired before it has been issued?
    lderr(cct) << __func__ << " possible clock skew, rotating keys expired way too early"
               << " (before " << issued_at_lower_bound << ")" << dendl;
  }
  if ((now > last_rotating_renew_sent) &&
      double(now - last_rotating_renew_sent) < 1) {
    ldout(cct, 10) << __func__ << " called too often (last: "
                   << last_rotating_renew_sent << "), skipping refresh" << dendl;
    return 0;
  }
  MAuth *m = new MAuth;
  m->protocol = auth->get_protocol();
  if (auth->build_rotating_request(m->auth_payload)) {
    last_rotating_renew_sent = now;
    _send_mon_message(m);
  } else {
    m->put();
  }
  return 0;
}