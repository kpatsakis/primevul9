parse_address (const char *s,
               char *token, size_t *tokenlen, size_t tokenmax,
	       char *comment, size_t *commentlen, size_t commentmax,
	       ADDRESS *addr)
{
  s = parse_mailboxdomain (s, ".\"(\\",
			   token, tokenlen, tokenmax,
			   comment, commentlen, commentmax);
  if (!s)
    return NULL;

  if (*s == '@')
  {
    if (*tokenlen < tokenmax)
      token[(*tokenlen)++] = '@';
    s = parse_domain (s + 1,
                      token, tokenlen, tokenmax,
                      comment, commentlen, commentmax);
    if (!s)
      return NULL;
  }

  terminate_string (token, *tokenlen, tokenmax);
  addr->mailbox = safe_strdup (token);

  if (*commentlen && !addr->personal)
  {
    terminate_string (comment, *commentlen, commentmax);
    addr->personal = safe_strdup (comment);
  }

  return s;
}