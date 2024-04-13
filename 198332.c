static int message_handler (BODY *a, STATE *s)
{
  struct stat st;
  BODY *b;
  LOFF_T off_start;
  int rc = 0;

  off_start = ftello (s->fpin);
  if (a->encoding == ENCBASE64 || a->encoding == ENCQUOTEDPRINTABLE ||
      a->encoding == ENCUUENCODED)
  {
    fstat (fileno (s->fpin), &st);
    b = mutt_new_body ();
    b->length = (LOFF_T) st.st_size;
    b->parts = mutt_parse_messageRFC822 (s->fpin, b);
  }
  else
    b = a;

  if (b->parts)
  {
    mutt_copy_hdr (s->fpin, s->fpout, off_start, b->parts->offset,
	(((s->flags & MUTT_WEED) || ((s->flags & (MUTT_DISPLAY|MUTT_PRINTING)) && option (OPTWEED))) ? (CH_WEED | CH_REORDER) : 0) |
	(s->prefix ? CH_PREFIX : 0) | CH_DECODE | CH_FROM |
	((s->flags & MUTT_DISPLAY) ? CH_DISPLAY : 0), s->prefix);

    if (s->prefix)
      state_puts (s->prefix, s);
    state_putc ('\n', s);

    rc = mutt_body_handler (b->parts, s);
  }

  if (a->encoding == ENCBASE64 || a->encoding == ENCQUOTEDPRINTABLE ||
      a->encoding == ENCUUENCODED)
    mutt_free_body (&b);

  return rc;
}