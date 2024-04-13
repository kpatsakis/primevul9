set_routerstatus_from_routerinfo(routerstatus_t *rs,
                                 node_t *node,
                                 routerinfo_t *ri,
                                 time_t now,
                                 int listbadexits)
{
  const or_options_t *options = get_options();
  uint32_t routerbw_kb = dirserv_get_credible_bandwidth_kb(ri);

  memset(rs, 0, sizeof(routerstatus_t));

  rs->is_authority =
    router_digest_is_trusted_dir(ri->cache_info.identity_digest);

  /* Already set by compute_performance_thresholds. */
  rs->is_exit = node->is_exit;
  rs->is_stable = node->is_stable =
    !dirserv_thinks_router_is_unreliable(now, ri, 1, 0);
  rs->is_fast = node->is_fast =
    !dirserv_thinks_router_is_unreliable(now, ri, 0, 1);
  rs->is_flagged_running = node->is_running; /* computed above */

  rs->is_valid = node->is_valid;

  if (node->is_fast && node->is_stable &&
      ((options->AuthDirGuardBWGuarantee &&
        routerbw_kb >= options->AuthDirGuardBWGuarantee/1000) ||
       routerbw_kb >= MIN(guard_bandwidth_including_exits_kb,
                          guard_bandwidth_excluding_exits_kb))) {
    long tk = rep_hist_get_weighted_time_known(
                                      node->identity, now);
    double wfu = rep_hist_get_weighted_fractional_uptime(
                                      node->identity, now);
    rs->is_possible_guard = (wfu >= guard_wfu && tk >= guard_tk) ? 1 : 0;
  } else {
    rs->is_possible_guard = 0;
  }

  rs->is_bad_exit = listbadexits && node->is_bad_exit;
  rs->is_hs_dir = node->is_hs_dir =
    dirserv_thinks_router_is_hs_dir(ri, node, now);

  rs->is_named = rs->is_unnamed = 0;

  rs->published_on = ri->cache_info.published_on;
  memcpy(rs->identity_digest, node->identity, DIGEST_LEN);
  memcpy(rs->descriptor_digest, ri->cache_info.signed_descriptor_digest,
         DIGEST_LEN);
  rs->addr = ri->addr;
  strlcpy(rs->nickname, ri->nickname, sizeof(rs->nickname));
  rs->or_port = ri->or_port;
  rs->dir_port = ri->dir_port;
  rs->is_v2_dir = ri->supports_tunnelled_dir_requests;
  if (options->AuthDirHasIPv6Connectivity == 1 &&
      !tor_addr_is_null(&ri->ipv6_addr) &&
      node->last_reachable6 >= now - REACHABLE_TIMEOUT) {
    /* We're configured as having IPv6 connectivity. There's an IPv6
       OR port and it's reachable so copy it to the routerstatus.  */
    tor_addr_copy(&rs->ipv6_addr, &ri->ipv6_addr);
    rs->ipv6_orport = ri->ipv6_orport;
  }

  if (options->TestingTorNetwork) {
    dirserv_set_routerstatus_testing(rs);
  }
}