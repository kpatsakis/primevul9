exit_policy_is_general_exit(smartlist_t *policy)
{
  static const int ports[] = { 80, 443, 6667 };
  int n_allowed = 0;
  int i;
  if (!policy) /*XXXX disallow NULL policies? */
    return 0;

  for (i = 0; i < 3; ++i) {
    SMARTLIST_FOREACH(policy, addr_policy_t *, p, {
      if (p->prt_min > ports[i] || p->prt_max < ports[i])
        continue; /* Doesn't cover our port. */
      if (p->maskbits > 8)
        continue; /* Narrower than a /8. */
      if (tor_addr_is_loopback(&p->addr))
        continue; /* 127.x or ::1. */
      /* We have a match that is at least a /8. */
      if (p->policy_type == ADDR_POLICY_ACCEPT) {
        ++n_allowed;
        break; /* stop considering this port */
      }
    });
  }
  return n_allowed >= 2;
}