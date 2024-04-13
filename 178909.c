addr_policy_free(addr_policy_t *p)
{
  if (p) {
    if (--p->refcnt <= 0) {
      if (p->is_canonical) {
        policy_map_ent_t search, *found;
        search.policy = p;
        found = HT_REMOVE(policy_map, &policy_root, &search);
        if (found) {
          tor_assert(p == found->policy);
          tor_free(found);
        }
      }
      tor_free(p);
    }
  }
}