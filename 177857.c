event_info *find_event(
    
  tm_event_t x)

  {
  event_info *ep;

  for (ep = event_hash[x % EVENT_HASH]; ep; ep = ep->e_next)
    {
    if (ep->e_event == x)
      break;
    }

  return ep;
  }