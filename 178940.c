policy_summary_split(smartlist_t *summary,
                     uint16_t prt_min, uint16_t prt_max)
{
  int start_at_index;

  int i = 0;
  /* XXXX Do a binary search if run time matters */
  while (AT(i)->prt_max < prt_min)
    i++;
  if (AT(i)->prt_min != prt_min) {
    policy_summary_item_t* new_item;
    new_item = policy_summary_item_split(AT(i), prt_min);
    smartlist_insert(summary, i+1, new_item);
    i++;
  }
  start_at_index = i;

  while (AT(i)->prt_max < prt_max)
    i++;
  if (AT(i)->prt_max != prt_max) {
    policy_summary_item_t* new_item;
    new_item = policy_summary_item_split(AT(i), prt_max+1);
    smartlist_insert(summary, i+1, new_item);
  }

  return start_at_index;
}