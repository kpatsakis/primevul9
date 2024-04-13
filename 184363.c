bool MonClient::_opened() const
{
  ceph_assert(monc_lock.is_locked());
  return active_con || _hunting();
}