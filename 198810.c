parse_quote (const char *s, char *token, size_t *tokenlen, size_t tokenmax)
{
  while (*s)
  {
    if (*tokenlen < tokenmax)
      token[*tokenlen] = *s;
    if (*s == '\\')
    {
      if (!*++s)
	break;

      if (*tokenlen < tokenmax)
	token[*tokenlen] = *s;
    }
    else if (*s == '"')
      return (s + 1);
    (*tokenlen)++;
    s++;
  }
  RFC822Error = ERR_MISMATCH_QUOTE;
  return NULL;
}