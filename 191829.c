dirserv_should_launch_reachability_test(const routerinfo_t *ri,
                                        const routerinfo_t *ri_old)
{
  if (!authdir_mode_handles_descs(get_options(), ri->purpose))
    return 0;
  if (!ri_old) {
    /* New router: Launch an immediate reachability test, so we will have an
     * opinion soon in case we're generating a consensus soon */
    return 1;
  }
  if (ri_old->is_hibernating && !ri->is_hibernating) {
    /* It just came out of hibernation; launch a reachability test */
    return 1;
  }
  if (! routers_have_same_or_addrs(ri, ri_old)) {
    /* Address or port changed; launch a reachability test */
    return 1;
  }
  return 0;
}