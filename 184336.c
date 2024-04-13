int MonClient::_cancel_mon_command(uint64_t tid)
{
  ceph_assert(monc_lock.is_locked());

  map<ceph_tid_t, MonCommand*>::iterator it = mon_commands.find(tid);
  if (it == mon_commands.end()) {
    ldout(cct, 10) << __func__ << " tid " << tid << " dne" << dendl;
    return -ENOENT;
  }

  ldout(cct, 10) << __func__ << " tid " << tid << dendl;

  MonCommand *cmd = it->second;
  _finish_command(cmd, -ETIMEDOUT, "");
  return 0;
}