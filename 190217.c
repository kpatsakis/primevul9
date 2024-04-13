exit_policy_is_general_exit_helper(smartlist_t *policy, int port)
{
  uint32_t mask, ip, i;
  /* Is this /8 rejected (1), or undecided (0)? */
  char subnet_status[256];

  memset(subnet_status, 0, sizeof(subnet_status));
  SMARTLIST_FOREACH(policy, addr_policy_t *, p, {
    if (tor_addr_family(&p->addr) != AF_INET)
      continue; /* IPv4 only for now */
    if (p->prt_min > port || p->prt_max < port)
      continue; /* Doesn't cover our port. */
    mask = 0;
    tor_assert(p->maskbits <= 32);

    if (p->maskbits)
      mask = UINT32_MAX<<(32-p->maskbits);
    ip = tor_addr_to_ipv4h(&p->addr);

    /* Calculate the first and last subnet that this exit policy touches
     * and set it as loop boundaries. */
    for (i = ((mask & ip)>>24); i <= (~((mask & ip) ^ mask)>>24); ++i) {
      tor_addr_t addr;
      if (subnet_status[i] != 0)
        continue; /* We already reject some part of this /8 */
      tor_addr_from_ipv4h(&addr, i<<24);
      if (tor_addr_is_internal(&addr, 0))
        continue; /* Local or non-routable addresses */
      if (p->policy_type == ADDR_POLICY_ACCEPT) {
        if (p->maskbits > 8)
          continue; /* Narrower than a /8. */
        /* We found an allowed subnet of at least size /8. Done
         * for this port! */
        return 1;
      } else if (p->policy_type == ADDR_POLICY_REJECT) {
        subnet_status[i] = 1;
      }
    }
  });
  return 0;
}