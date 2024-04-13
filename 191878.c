list_server_status_v1(smartlist_t *routers, char **router_status_out,
                      int for_controller)
{
  /* List of entries in a router-status style: An optional !, then an optional
   * equals-suffixed nickname, then a dollar-prefixed hexdigest. */
  smartlist_t *rs_entries;
  time_t now = time(NULL);
  time_t cutoff = now - ROUTER_MAX_AGE_TO_PUBLISH;
  const or_options_t *options = get_options();
  /* We include v2 dir auths here too, because they need to answer
   * controllers. Eventually we'll deprecate this whole function;
   * see also networkstatus_getinfo_by_purpose(). */
  int authdir = authdir_mode_publishes_statuses(options);
  tor_assert(router_status_out);

  rs_entries = smartlist_new();

  SMARTLIST_FOREACH_BEGIN(routers, routerinfo_t *, ri) {
    const node_t *node = node_get_by_id(ri->cache_info.identity_digest);
    tor_assert(node);
    if (authdir) {
      /* Update router status in routerinfo_t. */
      dirserv_set_router_is_running(ri, now);
    }
    if (for_controller) {
      char name_buf[MAX_VERBOSE_NICKNAME_LEN+2];
      char *cp = name_buf;
      if (!node->is_running)
        *cp++ = '!';
      router_get_verbose_nickname(cp, ri);
      smartlist_add(rs_entries, tor_strdup(name_buf));
    } else if (ri->cache_info.published_on >= cutoff) {
      smartlist_add(rs_entries, list_single_server_status(ri,
                                                          node->is_running));
    }
  } SMARTLIST_FOREACH_END(ri);

  *router_status_out = smartlist_join_strings(rs_entries, " ", 0, NULL);

  SMARTLIST_FOREACH(rs_entries, char *, cp, tor_free(cp));
  smartlist_free(rs_entries);

  return 0;
}