int MonConnection::authenticate(MAuthReply *m)
{
  ceph_assert(auth);
  if (!m->global_id) {
    ldout(cct, 1) << "peer sent an invalid global_id" << dendl;
  }
  if (m->global_id != global_id) {
    // it's a new session
    auth->reset();
    global_id = m->global_id;
    auth->set_global_id(global_id);
    ldout(cct, 10) << "my global_id is " << m->global_id << dendl;
  }
  auto p = m->result_bl.cbegin();
  int ret = auth->handle_response(m->result, p, nullptr, nullptr);
  if (ret == -EAGAIN) {
    auto ma = new MAuth;
    ma->protocol = auth->get_protocol();
    auth->prepare_build_request();
    auth->build_request(ma->auth_payload);
    con->send_message(ma);
  }
  return ret;
}