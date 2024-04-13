int tm_finalize(void)
  {
  event_info *e;
  int   i = 0;

  if (!init_done)
    return TM_BADINIT;

  while (event_count && (i < EVENT_HASH))
    {
    while ((e = event_hash[i]) != NULL)
      {
      del_event(e);
      }

    ++i; /* check next slot in hash table */
    }

  init_done = 0;

  return TM_SUCCESS; /* what else */
  }