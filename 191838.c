dirserv_single_reachability_test(time_t now, routerinfo_t *router)
{
  channel_t *chan = NULL;
  node_t *node = NULL;
  tor_addr_t router_addr;
  (void) now;

  tor_assert(router);
  node = node_get_mutable_by_id(router->cache_info.identity_digest);
  tor_assert(node);

  /* IPv4. */
  log_debug(LD_OR,"Testing reachability of %s at %s:%u.",
            router->nickname, fmt_addr32(router->addr), router->or_port);
  tor_addr_from_ipv4h(&router_addr, router->addr);
  chan = channel_tls_connect(&router_addr, router->or_port,
                             router->cache_info.identity_digest);
  if (chan) command_setup_channel(chan);

  /* Possible IPv6. */
  if (get_options()->AuthDirHasIPv6Connectivity == 1 &&
      !tor_addr_is_null(&router->ipv6_addr)) {
    char addrstr[TOR_ADDR_BUF_LEN];
    log_debug(LD_OR, "Testing reachability of %s at %s:%u.",
              router->nickname,
              tor_addr_to_str(addrstr, &router->ipv6_addr, sizeof(addrstr), 1),
              router->ipv6_orport);
    chan = channel_tls_connect(&router->ipv6_addr, router->ipv6_orport,
                               router->cache_info.identity_digest);
    if (chan) command_setup_channel(chan);
  }
}