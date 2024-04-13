ADDRESS *rfc822_cpy_adr (ADDRESS *addr, int prune)
{
  ADDRESS *top = NULL, *last = NULL;

  for (; addr; addr = addr->next)
  {
    if (prune && addr->group && (!addr->next || !addr->next->mailbox))
    {
      /* ignore this element of the list */
    }
    else if (last)
    {
      last->next = rfc822_cpy_adr_real (addr);
      last = last->next;
    }
    else
      top = last = rfc822_cpy_adr_real (addr);
  }
  return top;
}