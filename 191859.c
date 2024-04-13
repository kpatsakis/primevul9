dirserv_compute_performance_thresholds(digestmap_t *omit_as_sybil)
{
  int n_active, n_active_nonexit, n_familiar;
  uint32_t *uptimes, *bandwidths_kb, *bandwidths_excluding_exits_kb;
  long *tks;
  double *mtbfs, *wfus;
  smartlist_t *nodelist;
  time_t now = time(NULL);
  const or_options_t *options = get_options();

  /* Require mbw? */
  int require_mbw =
    (routers_with_measured_bw >
     options->MinMeasuredBWsForAuthToIgnoreAdvertised) ? 1 : 0;

  /* initialize these all here, in case there are no routers */
  stable_uptime = 0;
  stable_mtbf = 0;
  fast_bandwidth_kb = 0;
  guard_bandwidth_including_exits_kb = 0;
  guard_bandwidth_excluding_exits_kb = 0;
  guard_tk = 0;
  guard_wfu = 0;

  nodelist_assert_ok();
  nodelist = nodelist_get_list();

  /* Initialize arrays that will hold values for each router.  We'll
   * sort them and use that to compute thresholds. */
  n_active = n_active_nonexit = 0;
  /* Uptime for every active router. */
  uptimes = tor_calloc(smartlist_len(nodelist), sizeof(uint32_t));
  /* Bandwidth for every active router. */
  bandwidths_kb = tor_calloc(smartlist_len(nodelist), sizeof(uint32_t));
  /* Bandwidth for every active non-exit router. */
  bandwidths_excluding_exits_kb =
    tor_calloc(smartlist_len(nodelist), sizeof(uint32_t));
  /* Weighted mean time between failure for each active router. */
  mtbfs = tor_calloc(smartlist_len(nodelist), sizeof(double));
  /* Time-known for each active router. */
  tks = tor_calloc(smartlist_len(nodelist), sizeof(long));
  /* Weighted fractional uptime for each active router. */
  wfus = tor_calloc(smartlist_len(nodelist), sizeof(double));

  /* Now, fill in the arrays. */
  SMARTLIST_FOREACH_BEGIN(nodelist, node_t *, node) {
    if (options->BridgeAuthoritativeDir &&
        node->ri &&
        node->ri->purpose != ROUTER_PURPOSE_BRIDGE)
      continue;
    if (router_counts_toward_thresholds(node, now, omit_as_sybil,
                                        require_mbw)) {
      routerinfo_t *ri = node->ri;
      const char *id = node->identity;
      uint32_t bw_kb;
      /* resolve spurious clang shallow analysis null pointer errors */
      tor_assert(ri);
      node->is_exit = (!router_exit_policy_rejects_all(ri) &&
                       exit_policy_is_general_exit(ri->exit_policy));
      uptimes[n_active] = (uint32_t)real_uptime(ri, now);
      mtbfs[n_active] = rep_hist_get_stability(id, now);
      tks  [n_active] = rep_hist_get_weighted_time_known(id, now);
      bandwidths_kb[n_active] = bw_kb = dirserv_get_credible_bandwidth_kb(ri);
      if (!node->is_exit || node->is_bad_exit) {
        bandwidths_excluding_exits_kb[n_active_nonexit] = bw_kb;
        ++n_active_nonexit;
      }
      ++n_active;
    }
  } SMARTLIST_FOREACH_END(node);

  /* Now, compute thresholds. */
  if (n_active) {
    /* The median uptime is stable. */
    stable_uptime = median_uint32(uptimes, n_active);
    /* The median mtbf is stable, if we have enough mtbf info */
    stable_mtbf = median_double(mtbfs, n_active);
    /* The 12.5th percentile bandwidth is fast. */
    fast_bandwidth_kb = find_nth_uint32(bandwidths_kb, n_active, n_active/8);
    /* (Now bandwidths is sorted.) */
    if (fast_bandwidth_kb < RELAY_REQUIRED_MIN_BANDWIDTH/(2 * 1000))
      fast_bandwidth_kb = bandwidths_kb[n_active/4];
    guard_bandwidth_including_exits_kb =
      third_quartile_uint32(bandwidths_kb, n_active);
    guard_tk = find_nth_long(tks, n_active, n_active/8);
  }

  if (guard_tk > TIME_KNOWN_TO_GUARANTEE_FAMILIAR)
    guard_tk = TIME_KNOWN_TO_GUARANTEE_FAMILIAR;

  {
    /* We can vote on a parameter for the minimum and maximum. */
#define ABSOLUTE_MIN_VALUE_FOR_FAST_FLAG 4
    int32_t min_fast_kb, max_fast_kb, min_fast, max_fast;
    min_fast = networkstatus_get_param(NULL, "FastFlagMinThreshold",
      ABSOLUTE_MIN_VALUE_FOR_FAST_FLAG,
      ABSOLUTE_MIN_VALUE_FOR_FAST_FLAG,
      INT32_MAX);
    if (options->TestingTorNetwork) {
      min_fast = (int32_t)options->TestingMinFastFlagThreshold;
    }
    max_fast = networkstatus_get_param(NULL, "FastFlagMaxThreshold",
                                       INT32_MAX, min_fast, INT32_MAX);
    min_fast_kb = min_fast / 1000;
    max_fast_kb = max_fast / 1000;

    if (fast_bandwidth_kb < (uint32_t)min_fast_kb)
      fast_bandwidth_kb = min_fast_kb;
    if (fast_bandwidth_kb > (uint32_t)max_fast_kb)
      fast_bandwidth_kb = max_fast_kb;
  }
  /* Protect sufficiently fast nodes from being pushed out of the set
   * of Fast nodes. */
  if (options->AuthDirFastGuarantee &&
      fast_bandwidth_kb > options->AuthDirFastGuarantee/1000)
    fast_bandwidth_kb = (uint32_t)options->AuthDirFastGuarantee/1000;

  /* Now that we have a time-known that 7/8 routers are known longer than,
   * fill wfus with the wfu of every such "familiar" router. */
  n_familiar = 0;

  SMARTLIST_FOREACH_BEGIN(nodelist, node_t *, node) {
      if (router_counts_toward_thresholds(node, now,
                                          omit_as_sybil, require_mbw)) {
        routerinfo_t *ri = node->ri;
        const char *id = ri->cache_info.identity_digest;
        long tk = rep_hist_get_weighted_time_known(id, now);
        if (tk < guard_tk)
          continue;
        wfus[n_familiar++] = rep_hist_get_weighted_fractional_uptime(id, now);
      }
  } SMARTLIST_FOREACH_END(node);
  if (n_familiar)
    guard_wfu = median_double(wfus, n_familiar);
  if (guard_wfu > WFU_TO_GUARANTEE_GUARD)
    guard_wfu = WFU_TO_GUARANTEE_GUARD;

  enough_mtbf_info = rep_hist_have_measured_enough_stability();

  if (n_active_nonexit) {
    guard_bandwidth_excluding_exits_kb =
      find_nth_uint32(bandwidths_excluding_exits_kb,
                      n_active_nonexit, n_active_nonexit*3/4);
  }

  log_info(LD_DIRSERV,
      "Cutoffs: For Stable, %lu sec uptime, %lu sec MTBF. "
      "For Fast: %lu kilobytes/sec. "
      "For Guard: WFU %.03f%%, time-known %lu sec, "
      "and bandwidth %lu or %lu kilobytes/sec. "
      "We%s have enough stability data.",
      (unsigned long)stable_uptime,
      (unsigned long)stable_mtbf,
      (unsigned long)fast_bandwidth_kb,
      guard_wfu*100,
      (unsigned long)guard_tk,
      (unsigned long)guard_bandwidth_including_exits_kb,
      (unsigned long)guard_bandwidth_excluding_exits_kb,
      enough_mtbf_info ? "" : " don't");

  tor_free(uptimes);
  tor_free(mtbfs);
  tor_free(bandwidths_kb);
  tor_free(bandwidths_excluding_exits_kb);
  tor_free(tks);
  tor_free(wfus);
}