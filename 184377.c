void MonClient::_start_hunting()
{
  ceph_assert(!_hunting());
  // adjust timeouts if necessary
  if (!had_a_connection)
    return;
  reopen_interval_multiplier *= cct->_conf->mon_client_hunt_interval_backoff;
  if (reopen_interval_multiplier >
      cct->_conf->mon_client_hunt_interval_max_multiple) {
    reopen_interval_multiplier =
      cct->_conf->mon_client_hunt_interval_max_multiple;
  }
}