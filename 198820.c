add_addrspec (ADDRESS **top, ADDRESS **last, const char *phrase,
	      char *comment, size_t *commentlen, size_t commentmax)
{
  ADDRESS *cur = rfc822_new_address ();

  if (parse_addr_spec (phrase, comment, commentlen, commentmax, cur) == NULL)
  {
    rfc822_free_address (&cur);
    return;
  }

  if (*last)
    (*last)->next = cur;
  else
    *top = cur;
  *last = cur;
}