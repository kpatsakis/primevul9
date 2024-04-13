dirserv_count_measured_bws(const smartlist_t *routers)
{
  /* Initialize this first */
  routers_with_measured_bw = 0;

  /* Iterate over the routerlist and count measured bandwidths */
  SMARTLIST_FOREACH_BEGIN(routers, const routerinfo_t *, ri) {
    /* Check if we know a measured bandwidth for this one */
    if (dirserv_has_measured_bw(ri->cache_info.identity_digest)) {
      ++routers_with_measured_bw;
    }
  } SMARTLIST_FOREACH_END(ri);
}