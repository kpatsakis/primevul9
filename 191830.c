dirserv_get_bandwidth_for_router_kb(const routerinfo_t *ri)
{
  uint32_t bw_kb = 0;
  /*
   * Yeah, measured bandwidths in measured_bw_line_t are (implicitly
   * signed) longs and the ones router_get_advertised_bandwidth() returns
   * are uint32_t.
   */
  long mbw_kb = 0;

  if (ri) {
    /*
   * * First try to see if we have a measured bandwidth; don't bother with
     * as_of_out here, on the theory that a stale measured bandwidth is still
     * better to trust than an advertised one.
     */
    if (dirserv_query_measured_bw_cache_kb(ri->cache_info.identity_digest,
                                        &mbw_kb, NULL)) {
      /* Got one! */
      bw_kb = (uint32_t)mbw_kb;
    } else {
      /* If not, fall back to advertised */
      bw_kb = router_get_advertised_bandwidth(ri) / 1000;
    }
  }

  return bw_kb;
}