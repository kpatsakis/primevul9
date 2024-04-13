parse_literal (const char *s, char *token, size_t *tokenlen, size_t tokenmax)
{
  while (*s)
  {
    if (*s == '\\' || *s == '[')
    {
      RFC822Error = ERR_BAD_LITERAL;
      return NULL;
    }

    if (*tokenlen < tokenmax)
      token[(*tokenlen)++] = *s;

    if (*s == ']')
      return s + 1;

    s++;
  }
  RFC822Error = ERR_BAD_LITERAL;
  return NULL;
}