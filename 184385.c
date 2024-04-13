int MonClient::authenticate(double timeout)
{
  std::lock_guard lock(monc_lock);

  if (active_con) {
    ldout(cct, 5) << "already authenticated" << dendl;
    return 0;
  }
  sub.want("monmap", monmap.get_epoch() ? monmap.get_epoch() + 1 : 0, 0);
  sub.want("config", 0, 0);
  if (!_opened())
    _reopen_session();

  utime_t until = ceph_clock_now();
  until += timeout;
  if (timeout > 0.0)
    ldout(cct, 10) << "authenticate will time out at " << until << dendl;
  authenticate_err = 1;  // == in progress
  while (!active_con && authenticate_err >= 0) {
    if (timeout > 0.0) {
      int r = auth_cond.WaitUntil(monc_lock, until);
      if (r == ETIMEDOUT && !active_con) {
	ldout(cct, 0) << "authenticate timed out after " << timeout << dendl;
	authenticate_err = -r;
      }
    } else {
      auth_cond.Wait(monc_lock);
    }
  }

  if (active_con) {
    ldout(cct, 5) << __func__ << " success, global_id "
		  << active_con->get_global_id() << dendl;
    // active_con should not have been set if there was an error
    ceph_assert(authenticate_err >= 0);
    authenticated = true;
  }

  if (authenticate_err < 0 && auth_registry.no_keyring_disabled_cephx()) {
    lderr(cct) << __func__ << " NOTE: no keyring found; disabled cephx authentication" << dendl;
  }

  return authenticate_err;
}