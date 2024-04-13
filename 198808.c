parse_mailboxdomain (const char *s, const char *nonspecial,
		     char *mailbox, size_t *mailboxlen, size_t mailboxmax,
		     char *comment, size_t *commentlen, size_t commentmax)
{
  const char *ps;

  while (*s)
  {
    s = skip_email_wsp(s);
    if (! *s)
      return s;

    if (strchr (nonspecial, *s) == NULL && is_special (*s))
      return s;

    if (*s == '(')
    {
      if (*commentlen && *commentlen < commentmax)
	comment[(*commentlen)++] = ' ';
      ps = next_token (s, comment, commentlen, commentmax);
    }
    else
      ps = next_token (s, mailbox, mailboxlen, mailboxmax);
    if (!ps)
      return NULL;
    s = ps;
  }

  return s;
}