bool MonClient::ms_dispatch(Message *m)
{
  // we only care about these message types
  switch (m->get_type()) {
  case CEPH_MSG_MON_MAP:
  case CEPH_MSG_AUTH_REPLY:
  case CEPH_MSG_MON_SUBSCRIBE_ACK:
  case CEPH_MSG_MON_GET_VERSION_REPLY:
  case MSG_MON_COMMAND_ACK:
  case MSG_LOGACK:
  case MSG_CONFIG:
    break;
  case CEPH_MSG_PING:
    m->put();
    return true;
  default:
    return false;
  }

  std::lock_guard lock(monc_lock);

  if (_hunting()) {
    auto p = _find_pending_con(m->get_connection());
    if (p == pending_cons.end()) {
      // ignore any messages outside hunting sessions
      ldout(cct, 10) << "discarding stray monitor message " << *m << dendl;
      m->put();
      return true;
    }
  } else if (!active_con || active_con->get_con() != m->get_connection()) {
    // ignore any messages outside our session(s)
    ldout(cct, 10) << "discarding stray monitor message " << *m << dendl;
    m->put();
    return true;
  }

  switch (m->get_type()) {
  case CEPH_MSG_MON_MAP:
    handle_monmap(static_cast<MMonMap*>(m));
    if (passthrough_monmap) {
      return false;
    } else {
      m->put();
    }
    break;
  case CEPH_MSG_AUTH_REPLY:
    handle_auth(static_cast<MAuthReply*>(m));
    break;
  case CEPH_MSG_MON_SUBSCRIBE_ACK:
    handle_subscribe_ack(static_cast<MMonSubscribeAck*>(m));
    break;
  case CEPH_MSG_MON_GET_VERSION_REPLY:
    handle_get_version_reply(static_cast<MMonGetVersionReply*>(m));
    break;
  case MSG_MON_COMMAND_ACK:
    handle_mon_command_ack(static_cast<MMonCommandAck*>(m));
    break;
  case MSG_LOGACK:
    if (log_client) {
      log_client->handle_log_ack(static_cast<MLogAck*>(m));
      m->put();
      if (more_log_pending) {
	send_log();
      }
    } else {
      m->put();
    }
    break;
  case MSG_CONFIG:
    handle_config(static_cast<MConfig*>(m));
    break;
  }
  return true;
}