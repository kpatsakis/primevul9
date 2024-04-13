ADDRESS *rfc822_append (ADDRESS **a, ADDRESS *b, int prune)
{
  ADDRESS *tmp = *a;

  while (tmp && tmp->next)
    tmp = tmp->next;
  if (!b)
    return tmp;
  if (tmp)
    tmp->next = rfc822_cpy_adr (b, prune);
  else
    tmp = *a = rfc822_cpy_adr (b, prune);
  while (tmp && tmp->next)
    tmp = tmp->next;
  return tmp;
}