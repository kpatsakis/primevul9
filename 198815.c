int rfc822_remove_from_adrlist (ADDRESS **a, const char *mailbox)
{
  ADDRESS *p, *last = NULL, *t;
  int rv = -1;

  p = *a;
  last = NULL;
  while (p)
  {
    if (ascii_strcasecmp (mailbox, p->mailbox) == 0)
    {
      if (last)
	last->next = p->next;
      else
	(*a) = p->next;
      t = p;
      p = p->next;
      free_address (t);
      rv = 0;
    }
    else
    {
      last = p;
      p = p->next;
    }
  }

  return (rv);
}