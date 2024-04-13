static void state_prefix_put (const char *d, size_t dlen, STATE *s)
{
  if (s->prefix)
    while (dlen--)
      state_prefix_putc (*d++, s);
  else
    fwrite (d, dlen, 1, s->fpout);
}