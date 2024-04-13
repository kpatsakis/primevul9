dirserv_orconn_tls_done(const tor_addr_t *addr,
                        uint16_t or_port,
                        const char *digest_rcvd)
{
  node_t *node = NULL;
  tor_addr_port_t orport;
  routerinfo_t *ri = NULL;
  time_t now = time(NULL);
  tor_assert(addr);
  tor_assert(digest_rcvd);

  node = node_get_mutable_by_id(digest_rcvd);
  if (node == NULL || node->ri == NULL)
    return;
  ri = node->ri;

  tor_addr_copy(&orport.addr, addr);
  orport.port = or_port;
  if (router_has_orport(ri, &orport)) {
    /* Found the right router.  */
    if (!authdir_mode_bridge(get_options()) ||
        ri->purpose == ROUTER_PURPOSE_BRIDGE) {
      char addrstr[TOR_ADDR_BUF_LEN];
      /* This is a bridge or we're not a bridge authorititative --
         mark it as reachable.  */
      log_info(LD_DIRSERV, "Found router %s to be reachable at %s:%d. Yay.",
               router_describe(ri),
               tor_addr_to_str(addrstr, addr, sizeof(addrstr), 1),
               ri->or_port);
      if (tor_addr_family(addr) == AF_INET) {
        rep_hist_note_router_reachable(digest_rcvd, addr, or_port, now);
        node->last_reachable = now;
      } else if (tor_addr_family(addr) == AF_INET6) {
        /* No rephist for IPv6.  */
        node->last_reachable6 = now;
      }
    }
  }
}