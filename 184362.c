int MonClient::handle_auth_reply_more(
  Connection *con,
  AuthConnectionMeta *auth_meta,
  const bufferlist& bl,
  bufferlist *reply)
{
  std::lock_guard l(monc_lock);

  if (con->get_peer_type() == CEPH_ENTITY_TYPE_MON) {
    for (auto& i : pending_cons) {
      if (i.second.is_con(con)) {
	return i.second.handle_auth_reply_more(auth_meta, bl, reply);
      }
    }
    return -ENOENT;
  }

  // authorizer challenges
  if (!auth || !auth_meta->authorizer) {
    lderr(cct) << __func__ << " no authorizer?" << dendl;
    return -1;
  }
  auth_meta->authorizer->add_challenge(cct, bl);
  *reply = auth_meta->authorizer->bl;
  return 0;
}