int MonClient::get_monmap()
{
  ldout(cct, 10) << __func__ << dendl;
  std::lock_guard l(monc_lock);
  
  sub.want("monmap", 0, 0);
  if (!_opened())
    _reopen_session();

  while (want_monmap)
    map_cond.Wait(monc_lock);

  ldout(cct, 10) << __func__ << " done" << dendl;
  return 0;
}