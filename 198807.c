next_token (const char *s, char *token, size_t *tokenlen, size_t tokenmax)
{
  if (*s == '(')
    return (rfc822_parse_comment (s + 1, token, tokenlen, tokenmax));
  if (*s == '"')
    return (parse_quote (s + 1, token, tokenlen, tokenmax));
  if (*s && is_special (*s))
  {
    if (*tokenlen < tokenmax)
      token[(*tokenlen)++] = *s;
    return (s + 1);
  }
  while (*s)
  {
    if (is_email_wsp(*s) || is_special (*s))
      break;
    if (*tokenlen < tokenmax)
      token[(*tokenlen)++] = *s;
    s++;
  }
  return s;
}