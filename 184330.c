void MonClient::_reopen_session(int rank)
{
  ceph_assert(monc_lock.is_locked());
  ldout(cct, 10) << __func__ << " rank " << rank << dendl;

  active_con.reset();
  pending_cons.clear();

  _start_hunting();

  if (rank >= 0) {
    _add_conn(rank, global_id);
  } else {
    _add_conns(global_id);
  }

  // throw out old queued messages
  while (!waiting_for_session.empty()) {
    waiting_for_session.front()->put();
    waiting_for_session.pop_front();
  }

  // throw out version check requests
  while (!version_requests.empty()) {
    finisher.queue(version_requests.begin()->second->context, -EAGAIN);
    delete version_requests.begin()->second;
    version_requests.erase(version_requests.begin());
  }

  for (auto& c : pending_cons) {
    c.second.start(monmap.get_epoch(), entity_name);
  }

  if (sub.reload()) {
    _renew_subs();
  }
}