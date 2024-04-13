policy_summary_item_split(policy_summary_item_t* old, uint16_t new_starts)
{
  policy_summary_item_t* new;

  new = tor_malloc_zero(sizeof(policy_summary_item_t));
  new->prt_min = new_starts;
  new->prt_max = old->prt_max;
  new->reject_count = old->reject_count;
  new->accepted = old->accepted;

  old->prt_max = new_starts-1;

  tor_assert(old->prt_min <= old->prt_max);
  tor_assert(new->prt_min <= new->prt_max);
  return new;
}