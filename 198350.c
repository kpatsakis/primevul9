static void enriched_puts (const char *s, struct enriched_state *stte)
{
  const char *c;

  if (stte->buff_len < stte->buff_used + mutt_strlen (s))
  {
    stte->buff_len += LONG_STRING;
    safe_realloc (&stte->buffer, (stte->buff_len + 1) * sizeof (wchar_t));
  }
  c = s;
  while (*c)
  {
    stte->buffer[stte->buff_used++] = (wchar_t) *c;
    c++;
  }
}