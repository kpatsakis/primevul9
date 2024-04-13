addr_policy_covers(addr_policy_t *a, addr_policy_t *b)
{
  /* We can ignore accept/reject, since "accept *:80, reject *:80" reduces
   * to "accept *:80". */
  if (a->maskbits > b->maskbits) {
    /* a has more fixed bits than b; it can't possibly cover b. */
    return 0;
  }
  if (tor_addr_compare_masked(&a->addr, &b->addr, a->maskbits, CMP_EXACT)) {
    /* There's a fixed bit in a that's set differently in b. */
    return 0;
  }
  return (a->prt_min <= b->prt_min && a->prt_max >= b->prt_max);
}