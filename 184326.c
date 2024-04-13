void MonClient::_renew_subs()
{
  ceph_assert(monc_lock.is_locked());
  if (!sub.have_new()) {
    ldout(cct, 10) << __func__ << " - empty" << dendl;
    return;
  }

  ldout(cct, 10) << __func__ << dendl;
  if (!_opened())
    _reopen_session();
  else {
    MMonSubscribe *m = new MMonSubscribe;
    m->what = sub.get_subs();
    m->hostname = ceph_get_short_hostname();
    _send_mon_message(m);
    sub.renewed();
  }
}