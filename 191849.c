directory_fetches_from_authorities(const or_options_t *options)
{
  const routerinfo_t *me;
  uint32_t addr;
  int refuseunknown;
  if (options->FetchDirInfoEarly)
    return 1;
  if (options->BridgeRelay == 1)
    return 0;
  if (server_mode(options) &&
      router_pick_published_address(options, &addr, 1) < 0)
    return 1; /* we don't know our IP address; ask an authority. */
  refuseunknown = ! router_my_exit_policy_is_reject_star() &&
    should_refuse_unknown_exits(options);
  if (!dir_server_mode(options) && !refuseunknown)
    return 0;
  if (!server_mode(options) || !advertised_server_mode())
    return 0;
  me = router_get_my_routerinfo();
  if (!me || (!me->supports_tunnelled_dir_requests && !refuseunknown))
    return 0; /* if we don't service directory requests, return 0 too */
  return 1;
}