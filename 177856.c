void add_event(
    
  tm_event_t  event,
  tm_node_id  node,
  int         type,
  void       *info)
  {
  event_info  *ep, **head;

  ep = (event_info *)calloc(1, sizeof(event_info));
  assert(ep != NULL);

  head = &event_hash[event % EVENT_HASH];
  ep->e_event = event;
  ep->e_node = node;
  ep->e_mtype = type;
  ep->e_info = info;
  ep->e_next = *head;
  ep->e_prev = NULL;

  if (*head)
    (*head)->e_prev = ep;

  *head = ep;

  event_count++;

  return;
  }