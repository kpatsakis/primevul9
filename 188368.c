char *imap_next_word (char *s)
{
  int quoted = 0;

  while (*s)
  {
    if (*s == '\\')
    {
      s++;
      if (*s)
	s++;
      continue;
    }
    if (*s == '\"')
      quoted = quoted ? 0 : 1;
    if (!quoted && ISSPACE (*s))
      break;
    s++;
  }

  SKIPWS (s);
  return s;
}