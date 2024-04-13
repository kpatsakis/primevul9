tm_event_t new_event(void)

  {
  static tm_event_t next_event = TM_NULL_EVENT + 1;
  event_info  *ep;
  tm_event_t  ret;

  if (next_event == INT_MAX)
    next_event = TM_NULL_EVENT + 1;

  for (;;)
    {
    ret = next_event++;

    for (ep = event_hash[ret % EVENT_HASH]; ep; ep = ep->e_next)
      {
      if (ep->e_event == ret)
        break; /* inner loop: this number is in use */
      }

    if (ep == NULL)
      break;  /* this number is not in use */
    }

  return ret;
  }