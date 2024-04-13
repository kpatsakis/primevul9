int MonClient::wait_auth_rotating(double timeout)
{
  std::lock_guard l(monc_lock);
  utime_t now = ceph_clock_now();
  utime_t until = now;
  until += timeout;

  // Must be initialized
  ceph_assert(auth != nullptr);

  if (auth->get_protocol() == CEPH_AUTH_NONE)
    return 0;
  
  if (!rotating_secrets)
    return 0;

  while (auth_principal_needs_rotating_keys(entity_name) &&
	 rotating_secrets->need_new_secrets(now)) {
    if (now >= until) {
      ldout(cct, 0) << __func__ << " timed out after " << timeout << dendl;
      return -ETIMEDOUT;
    }
    ldout(cct, 10) << __func__ << " waiting (until " << until << ")" << dendl;
    auth_cond.WaitUntil(monc_lock, until);
    now = ceph_clock_now();
  }
  ldout(cct, 10) << __func__ << " done" << dendl;
  return 0;
}