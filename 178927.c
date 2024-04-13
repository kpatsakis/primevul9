load_policy_from_option(config_line_t *config, smartlist_t **policy,
                        int assume_action)
{
  int r;
  addr_policy_list_free(*policy);
  *policy = NULL;
  r = parse_addr_policy(config, policy, assume_action);
  if (r < 0) {
    return -1;
  }
  if (*policy) {
    SMARTLIST_FOREACH_BEGIN(*policy, addr_policy_t *, n) {
      /* ports aren't used in these. */
      if (n->prt_min > 1 || n->prt_max != 65535) {
        addr_policy_t newp, *c;
        memcpy(&newp, n, sizeof(newp));
        newp.prt_min = 1;
        newp.prt_max = 65535;
        newp.is_canonical = 0;
        c = addr_policy_get_canonical_entry(&newp);
        SMARTLIST_REPLACE_CURRENT(*policy, n, c);
        addr_policy_free(n);
      }
    } SMARTLIST_FOREACH_END(n);
  }
  return 0;
}