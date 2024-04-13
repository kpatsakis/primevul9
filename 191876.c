get_possible_sybil_list(const smartlist_t *routers)
{
  const or_options_t *options = get_options();
  digestmap_t *omit_as_sybil;
  smartlist_t *routers_by_ip = smartlist_new();
  uint32_t last_addr;
  int addr_count;
  /* Allow at most this number of Tor servers on a single IP address, ... */
  int max_with_same_addr = options->AuthDirMaxServersPerAddr;
  /* ... unless it's a directory authority, in which case allow more. */
  int max_with_same_addr_on_authority = options->AuthDirMaxServersPerAuthAddr;
  if (max_with_same_addr <= 0)
    max_with_same_addr = INT_MAX;
  if (max_with_same_addr_on_authority <= 0)
    max_with_same_addr_on_authority = INT_MAX;

  smartlist_add_all(routers_by_ip, routers);
  smartlist_sort(routers_by_ip, compare_routerinfo_by_ip_and_bw_);
  omit_as_sybil = digestmap_new();

  last_addr = 0;
  addr_count = 0;
  SMARTLIST_FOREACH_BEGIN(routers_by_ip, routerinfo_t *, ri) {
      if (last_addr != ri->addr) {
        last_addr = ri->addr;
        addr_count = 1;
      } else if (++addr_count > max_with_same_addr) {
        if (!router_addr_is_trusted_dir(ri->addr) ||
            addr_count > max_with_same_addr_on_authority)
          digestmap_set(omit_as_sybil, ri->cache_info.identity_digest, ri);
      }
  } SMARTLIST_FOREACH_END(ri);

  smartlist_free(routers_by_ip);
  return omit_as_sybil;
}