policy_is_reject_star(const smartlist_t *policy)
{
  if (!policy) /*XXXX disallow NULL policies? */
    return 1;
  SMARTLIST_FOREACH(policy, addr_policy_t *, p, {
    if (p->policy_type == ADDR_POLICY_ACCEPT)
      return 0;
    else if (p->policy_type == ADDR_POLICY_REJECT &&
             p->prt_min <= 1 && p->prt_max == 65535 &&
             p->maskbits == 0)
      return 1;
  });
  return 1;
}