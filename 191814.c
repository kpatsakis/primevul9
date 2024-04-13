dirserv_set_router_is_running(routerinfo_t *router, time_t now)
{
  /*XXXX This function is a mess.  Separate out the part that calculates
    whether it's reachable and the part that tells rephist that the router was
    unreachable.
   */
  int answer;
  const or_options_t *options = get_options();
  node_t *node = node_get_mutable_by_id(router->cache_info.identity_digest);
  tor_assert(node);

  if (router_is_me(router)) {
    /* We always know if we are down ourselves. */
    answer = ! we_are_hibernating();
  } else if (router->is_hibernating &&
             (router->cache_info.published_on +
              HIBERNATION_PUBLICATION_SKEW) > node->last_reachable) {
    /* A hibernating router is down unless we (somehow) had contact with it
     * since it declared itself to be hibernating. */
    answer = 0;
  } else if (options->AssumeReachable) {
    /* If AssumeReachable, everybody is up unless they say they are down! */
    answer = 1;
  } else {
    /* Otherwise, a router counts as up if we found all announced OR
       ports reachable in the last REACHABLE_TIMEOUT seconds.

       XXX prop186 For now there's always one IPv4 and at most one
       IPv6 OR port.

       If we're not on IPv6, don't consider reachability of potential
       IPv6 OR port since that'd kill all dual stack relays until a
       majority of the dir auths have IPv6 connectivity. */
    answer = (now < node->last_reachable + REACHABLE_TIMEOUT &&
              (options->AuthDirHasIPv6Connectivity != 1 ||
               tor_addr_is_null(&router->ipv6_addr) ||
               now < node->last_reachable6 + REACHABLE_TIMEOUT));
  }

  if (!answer && running_long_enough_to_decide_unreachable()) {
    /* Not considered reachable. tell rephist about that.

       Because we launch a reachability test for each router every
       REACHABILITY_TEST_CYCLE_PERIOD seconds, then the router has probably
       been down since at least that time after we last successfully reached
       it.

       XXX ipv6
     */
    time_t when = now;
    if (node->last_reachable &&
        node->last_reachable + REACHABILITY_TEST_CYCLE_PERIOD < now)
      when = node->last_reachable + REACHABILITY_TEST_CYCLE_PERIOD;
    rep_hist_note_router_unreachable(router->cache_info.identity_digest, when);
  }

  node->is_running = answer;
}