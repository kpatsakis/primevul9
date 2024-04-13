policy_hash(policy_map_ent_t *ent)
{
  addr_policy_t *a = ent->policy;
  unsigned int r;
  if (a->is_private)
    r = 0x1234abcd;
  else
    r = tor_addr_hash(&a->addr);
  r += a->prt_min << 8;
  r += a->prt_max << 16;
  r += a->maskbits;
  if (a->policy_type == ADDR_POLICY_REJECT)
    r ^= 0xffffffff;

  return r;
}