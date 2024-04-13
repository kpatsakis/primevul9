parse_route_addr (const char *s,
		  char *comment, size_t *commentlen, size_t commentmax,
		  ADDRESS *addr)
{
  char token[LONG_STRING];
  size_t tokenlen = 0;

  s = skip_email_wsp(s);

  /* find the end of the route */
  if (*s == '@')
  {
    while (s && *s == '@')
    {
      if (tokenlen < sizeof (token) - 1)
	token[tokenlen++] = '@';
      s = parse_mailboxdomain (s + 1, ",.\\[](", token,
			       &tokenlen, sizeof (token) - 1,
			       comment, commentlen, commentmax);
    }
    if (!s || *s != ':')
    {
      RFC822Error = ERR_BAD_ROUTE;
      return NULL; /* invalid route */
    }

    if (tokenlen < sizeof (token) - 1)
      token[tokenlen++] = ':';
    s++;
  }

  if ((s = parse_address (s, token, &tokenlen, sizeof (token) - 1, comment, commentlen, commentmax, addr)) == NULL)
    return NULL;

  if (*s != '>')
  {
    RFC822Error = ERR_BAD_ROUTE_ADDR;
    return NULL;
  }

  if (!addr->mailbox)
    addr->mailbox = safe_strdup ("@");

  s++;
  return s;
}