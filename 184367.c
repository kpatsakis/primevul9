MonConnection& MonClient::_add_conn(unsigned rank, uint64_t global_id)
{
  auto peer = monmap.get_addrs(rank);
  auto conn = messenger->connect_to_mon(peer);
  MonConnection mc(cct, conn, global_id, &auth_registry);
  auto inserted = pending_cons.insert(make_pair(peer, move(mc)));
  ldout(cct, 10) << "picked mon." << monmap.get_name(rank)
                 << " con " << conn
                 << " addr " << peer
                 << dendl;
  return inserted.first->second;
}