dirserv_has_measured_bw(const char *node_id)
{
  return dirserv_query_measured_bw_cache_kb(node_id, NULL, NULL);
}