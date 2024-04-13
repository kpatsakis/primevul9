static int text_plain_handler (BODY *b, STATE *s)
{
  char *buf = NULL;
  size_t l = 0, sz = 0;

  while ((buf = mutt_read_line (buf, &sz, s->fpin, NULL, 0)))
  {
    if (mutt_strcmp (buf, "-- ") != 0 && option (OPTTEXTFLOWED))
    {
      l = mutt_strlen (buf);
      while (l > 0 && buf[l-1] == ' ')
	buf[--l] = 0;
    }
    if (s->prefix)
      state_puts (s->prefix, s);
    state_puts (buf, s);
    state_putc ('\n', s);
  }

  FREE (&buf);
  return 0;
}