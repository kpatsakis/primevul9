void MonClient::shutdown()
{
  ldout(cct, 10) << __func__ << dendl;
  monc_lock.Lock();
  stopping = true;
  while (!version_requests.empty()) {
    version_requests.begin()->second->context->complete(-ECANCELED);
    ldout(cct, 20) << __func__ << " canceling and discarding version request "
		   << version_requests.begin()->second << dendl;
    delete version_requests.begin()->second;
    version_requests.erase(version_requests.begin());
  }
  while (!mon_commands.empty()) {
    auto tid = mon_commands.begin()->first;
    _cancel_mon_command(tid);
  }
  while (!waiting_for_session.empty()) {
    ldout(cct, 20) << __func__ << " discarding pending message " << *waiting_for_session.front() << dendl;
    waiting_for_session.front()->put();
    waiting_for_session.pop_front();
  }

  active_con.reset();
  pending_cons.clear();
  auth.reset();

  monc_lock.Unlock();

  if (initialized) {
    finisher.wait_for_empty();
    finisher.stop();
    initialized = false;
  }
  monc_lock.Lock();
  timer.shutdown();
  stopping = false;
  monc_lock.Unlock();
}