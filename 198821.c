rfc822_parse_comment (const char *s,
                      char *comment, size_t *commentlen, size_t commentmax)
{
  int level = 1;

  while (*s && level)
  {
    if (*s == '(')
      level++;
    else if (*s == ')')
    {
      if (--level == 0)
      {
	s++;
	break;
      }
    }
    else if (*s == '\\')
    {
      if (!*++s)
	break;
    }
    if (*commentlen < commentmax)
      comment[(*commentlen)++] = *s;
    s++;
  }
  if (level)
  {
    RFC822Error = ERR_MISMATCH_PAREN;
    return NULL;
  }
  return s;
}