policy_summary_add_item(smartlist_t *summary, addr_policy_t *p)
{
  if (p->policy_type == ADDR_POLICY_ACCEPT) {
    if (p->maskbits == 0) {
      policy_summary_accept(summary, p->prt_min, p->prt_max);
    }
  } else if (p->policy_type == ADDR_POLICY_REJECT) {

     int is_private = 0;
     int i;
     for (i = 0; private_nets[i]; ++i) {
       tor_addr_t addr;
       maskbits_t maskbits;
       if (tor_addr_parse_mask_ports(private_nets[i], &addr,
                                  &maskbits, NULL, NULL)<0) {
         tor_assert(0);
       }
       if (tor_addr_compare(&p->addr, &addr, CMP_EXACT) == 0 &&
           p->maskbits == maskbits) {
         is_private = 1;
         break;
       }
     }

     if (!is_private) {
       policy_summary_reject(summary, p->maskbits, p->prt_min, p->prt_max);
     }
  } else
    tor_assert(0);
}