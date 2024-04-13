policy_summary_create(void)
{
  smartlist_t *summary;
  policy_summary_item_t* item;

  item = tor_malloc_zero(sizeof(policy_summary_item_t));
  item->prt_min = 1;
  item->prt_max = 65535;
  item->reject_count = 0;
  item->accepted = 0;

  summary = smartlist_create();
  smartlist_add(summary, item);

  return summary;
}