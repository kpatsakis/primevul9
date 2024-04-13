parse_domain (const char *s,
              char *mailbox, size_t *mailboxlen, size_t mailboxmax,
              char *comment, size_t *commentlen, size_t commentmax)
{
  const char *ps;
  const char *nonspecial;
  int domain_literal = 0;

  while (*s)
  {
    s = skip_email_wsp(s);
    if (! *s)
      return s;

    if (*s == '(')
    {
      if (*commentlen && *commentlen < commentmax)
	comment[(*commentlen)++] = ' ';
      ps = next_token (s, comment, commentlen, commentmax);
    }
    else
    {
      if (*s == '[')
      {
        domain_literal = 1;
        if (*mailboxlen < mailboxmax)
          mailbox[(*mailboxlen)++] = '[';
        s++;
        nonspecial = "@.,:;<>\"()";
      }
      else
        nonspecial = ".([]\\";

      s = parse_mailboxdomain (s, nonspecial,
                               mailbox, mailboxlen, mailboxmax,
                               comment, commentlen, commentmax);
      if (domain_literal)
      {
        if (!s || *s != ']')
        {
          RFC822Error = ERR_BAD_LITERAL;
          return NULL;
        }

        if (*mailboxlen < mailboxmax)
          mailbox[(*mailboxlen)++] = ']';
        s++;
      }
      return s;
    }
    if (!ps)
      return NULL;
    s = ps;
  }

  return s;
}