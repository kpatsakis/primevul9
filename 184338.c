void MonClient::_add_conns(uint64_t global_id)
{
  uint16_t min_priority = std::numeric_limits<uint16_t>::max();
  for (const auto& m : monmap.mon_info) {
    if (m.second.priority < min_priority) {
      min_priority = m.second.priority;
    }
  }
  vector<unsigned> ranks;
  for (const auto& m : monmap.mon_info) {
    if (m.second.priority == min_priority) {
      ranks.push_back(monmap.get_rank(m.first));
    }
  }
  std::random_device rd;
  std::mt19937 rng(rd());
  std::shuffle(ranks.begin(), ranks.end(), rng);
  unsigned n = cct->_conf->mon_client_hunt_parallel;
  if (n == 0 || n > ranks.size()) {
    n = ranks.size();
  }
  for (unsigned i = 0; i < n; i++) {
    _add_conn(ranks[i], global_id);
  }
}