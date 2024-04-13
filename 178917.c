addr_policy_intersects(addr_policy_t *a, addr_policy_t *b)
{
  maskbits_t minbits;
  /* All the bits we care about are those that are set in both
   * netmasks.  If they are equal in a and b's networkaddresses
   * then the networks intersect.  If there is a difference,
   * then they do not. */
  if (a->maskbits < b->maskbits)
    minbits = a->maskbits;
  else
    minbits = b->maskbits;
  if (tor_addr_compare_masked(&a->addr, &b->addr, minbits, CMP_EXACT))
    return 0;
  if (a->prt_max < b->prt_min || b->prt_max < a->prt_min)
    return 0;
  return 1;
}