router_add_exit_policy(routerinfo_t *router, directory_token_t *tok)
{
  addr_policy_t *newe;
  newe = router_parse_addr_policy(tok);
  if (!newe)
    return -1;
  if (! router->exit_policy)
    router->exit_policy = smartlist_create();

  if (((tok->tp == K_ACCEPT6 || tok->tp == K_REJECT6) &&
       tor_addr_family(&newe->addr) == AF_INET)
      ||
      ((tok->tp == K_ACCEPT || tok->tp == K_REJECT) &&
       tor_addr_family(&newe->addr) == AF_INET6)) {
    log_warn(LD_DIR, "Mismatch between field type and address type in exit "
             "policy");
    addr_policy_free(newe);
    return -1;
  }

  smartlist_add(router->exit_policy, newe);

  return 0;
}