router_counts_toward_thresholds(const node_t *node, time_t now,
                                const digestmap_t *omit_as_sybil,
                                int require_mbw)
{
  /* Have measured bw? */
  int have_mbw =
    dirserv_has_measured_bw(node->identity);
  uint64_t min_bw_kb = ABSOLUTE_MIN_BW_VALUE_TO_CONSIDER_KB;
  const or_options_t *options = get_options();

  if (options->TestingTorNetwork) {
    min_bw_kb = (int64_t)options->TestingMinExitFlagThreshold / 1000;
  }

  return node->ri && router_is_active(node->ri, node, now) &&
    !digestmap_get(omit_as_sybil, node->identity) &&
    (dirserv_get_credible_bandwidth_kb(node->ri) >= min_bw_kb) &&
    (have_mbw || !require_mbw);
}