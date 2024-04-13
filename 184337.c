int MonConnection::handle_auth_reply_more(
  AuthConnectionMeta *auth_meta,
  const bufferlist& bl,
  bufferlist *reply)
{
  ldout(cct, 10) << __func__ << " payload " << bl.length() << dendl;
  ldout(cct, 30) << __func__ << " got\n";
  bl.hexdump(*_dout);
  *_dout << dendl;

  auto p = bl.cbegin();
  ldout(cct, 10) << __func__ << " payload_len " << bl.length() << dendl;
  int r = auth->handle_response(0, p, &auth_meta->session_key,
				&auth_meta->connection_secret);
  if (r == -EAGAIN) {
    auth->prepare_build_request();
    auth->build_request(*reply);
    ldout(cct, 10) << __func__ << " responding with " << reply->length()
		   << " bytes" << dendl;
    r = 0;
  } else if (r < 0) {
    lderr(cct) << __func__ << " handle_response returned " << r << dendl;
  } else {
    ldout(cct, 10) << __func__ << " authenticated!" << dendl;
    // FIXME
    ceph_abort(cct, "write me");
  }
  return r;
}