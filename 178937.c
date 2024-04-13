policy_expand_private(smartlist_t **policy)
{
  uint16_t port_min, port_max;

  int i;
  smartlist_t *tmp;

  if (!*policy) /*XXXX disallow NULL policies? */
    return;

  tmp = smartlist_create();

  SMARTLIST_FOREACH(*policy, addr_policy_t *, p,
  {
     if (! p->is_private) {
       smartlist_add(tmp, p);
       continue;
     }
     for (i = 0; private_nets[i]; ++i) {
       addr_policy_t policy;
       memcpy(&policy, p, sizeof(addr_policy_t));
       policy.is_private = 0;
       policy.is_canonical = 0;
       if (tor_addr_parse_mask_ports(private_nets[i], &policy.addr,
                                  &policy.maskbits, &port_min, &port_max)<0) {
         tor_assert(0);
       }
       smartlist_add(tmp, addr_policy_get_canonical_entry(&policy));
     }
     addr_policy_free(p);
  });

  smartlist_free(*policy);
  *policy = tmp;
}