router_is_active(const routerinfo_t *ri, const node_t *node, time_t now)
{
  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  if (ri->cache_info.published_on < cutoff) {
    return 0;
  }
  if (!node->is_running || !node->is_valid || ri->is_hibernating) {
    return 0;
  }
  /* Only require bandwith capacity in non-test networks, or
   * if TestingTorNetwork, and TestingMinExitFlagThreshold is non-zero */
  if (!ri->bandwidthcapacity) {
    if (get_options()->TestingTorNetwork) {
      if (get_options()->TestingMinExitFlagThreshold > 0) {
        /* If we're in a TestingTorNetwork, and TestingMinExitFlagThreshold is,
         * then require bandwidthcapacity */
        return 0;
      }
    } else {
      /* If we're not in a TestingTorNetwork, then require bandwidthcapacity */
      return 0;
    }
  }
  return 1;
}