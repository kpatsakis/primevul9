compare_known_tor_addr_to_addr_policy_noport(const tor_addr_t *addr,
                                             const smartlist_t *policy)
{
  /* We look to see if there's a definite match.  If so, we return that
     match's value, unless there's an intervening possible match that says
     something different. */
  int maybe_accept = 0, maybe_reject = 0;

  SMARTLIST_FOREACH_BEGIN(policy, addr_policy_t *, tmpe) {
    if (!tor_addr_compare_masked(addr, &tmpe->addr, tmpe->maskbits,
                                 CMP_EXACT)) {
      if (tmpe->prt_min <= 1 && tmpe->prt_max >= 65535) {
        /* Definitely matches, since it covers all ports. */
        if (tmpe->policy_type == ADDR_POLICY_ACCEPT) {
          /* If we already hit a clause that might trigger a 'reject', than we
           * can't be sure of this certain 'accept'.*/
          return maybe_reject ? ADDR_POLICY_PROBABLY_ACCEPTED :
            ADDR_POLICY_ACCEPTED;
        } else {
          return maybe_accept ? ADDR_POLICY_PROBABLY_REJECTED :
            ADDR_POLICY_REJECTED;
        }
      } else {
        /* Might match. */
        if (tmpe->policy_type == ADDR_POLICY_REJECT)
          maybe_reject = 1;
        else
          maybe_accept = 1;
      }
    }
  } SMARTLIST_FOREACH_END(tmpe);

  /* accept all by default. */
  return maybe_reject ? ADDR_POLICY_PROBABLY_ACCEPTED : ADDR_POLICY_ACCEPTED;
}