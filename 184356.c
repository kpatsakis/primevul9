MonConnection::MonConnection(
  CephContext *cct, ConnectionRef con, uint64_t global_id,
  AuthRegistry *ar)
  : cct(cct), con(con), global_id(global_id), auth_registry(ar)
{}