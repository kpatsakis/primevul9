dirserv_thinks_router_is_unreliable(time_t now,
                                    routerinfo_t *router,
                                    int need_uptime, int need_capacity)
{
  if (need_uptime) {
    if (!enough_mtbf_info) {
      /* XXXX We should change the rule from
       * "use uptime if we don't have mtbf data" to "don't advertise Stable on
       * v3 if we don't have enough mtbf data."  Or maybe not, since if we ever
       * hit a point where we need to reset a lot of authorities at once,
       * none of them would be in a position to declare Stable.
       */
      long uptime = real_uptime(router, now);
      if ((unsigned)uptime < stable_uptime &&
          (unsigned)uptime < UPTIME_TO_GUARANTEE_STABLE)
        return 1;
    } else {
      double mtbf =
        rep_hist_get_stability(router->cache_info.identity_digest, now);
      if (mtbf < stable_mtbf &&
          mtbf < MTBF_TO_GUARANTEE_STABLE)
        return 1;
    }
  }
  if (need_capacity) {
    uint32_t bw_kb = dirserv_get_credible_bandwidth_kb(router);
    if (bw_kb < fast_bandwidth_kb)
      return 1;
  }
  return 0;
}