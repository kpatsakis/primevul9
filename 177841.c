void del_event(
    
  event_info *ep)

  {
  /* unlink event from hash list */
  if (ep->e_prev)
    ep->e_prev->e_next = ep->e_next;
  else
    event_hash[ep->e_event % EVENT_HASH] = ep->e_next;

  if (ep->e_next)
    ep->e_next->e_prev = ep->e_prev;

  /*
  ** Free any memory saved with the event.  This depends
  ** on whay type of event it is.
  */
  switch (ep->e_mtype)
    {

    case TM_INIT:

    case TM_SPAWN:

    case TM_SIGNAL:

    case TM_OBIT:

    case TM_POSTINFO:
      break;

    case TM_TASKS:

    case TM_GETINFO:

    case TM_RESOURCES:
      free(ep->e_info);
      break;

    default:
      TM_DBPRT(("del_event: unknown event command %d\n", ep->e_mtype))
      break;
    }

  free(ep);

  if (--event_count == 0)
    {
    close(local_conn);
    local_conn = -1;
    }

  return;
  }