static int text_enriched_handler (BODY *a, STATE *s)
{
  enum {
    TEXT, LANGLE, TAG, BOGUS_TAG, NEWLINE, ST_EOF, DONE
  } state = TEXT;

  LOFF_T bytes = a->length;
  struct enriched_state stte;
  wchar_t wc = 0;
  int tag_len = 0;
  wchar_t tag[LONG_STRING + 1];

  memset (&stte, 0, sizeof (stte));
  stte.s = s;
  stte.WrapMargin = ((s->flags & MUTT_DISPLAY) ? (MuttIndexWindow->cols-4) :
                     ((MuttIndexWindow->cols-4)<72)?(MuttIndexWindow->cols-4):72);
  stte.line_max = stte.WrapMargin * 4;
  stte.line = (wchar_t *) safe_calloc (1, (stte.line_max + 1) * sizeof (wchar_t));
  stte.param = (wchar_t *) safe_calloc (1, (STRING) * sizeof (wchar_t));

  stte.param_len = STRING;
  stte.param_used = 0;

  if (s->prefix)
  {
    state_puts (s->prefix, s);
    stte.indent_len += mutt_strlen (s->prefix);
  }

  while (state != DONE)
  {
    if (state != ST_EOF)
    {
      if (!bytes || (wc = fgetwc (s->fpin)) == WEOF)
	state = ST_EOF;
      else
	bytes--;
    }

    switch (state)
    {
      case TEXT :
	switch (wc)
	{
	  case '<' :
	    state = LANGLE;
	    break;

	  case '\n' :
	    if (stte.tag_level[RICH_NOFILL])
	    {
	      enriched_flush (&stte, 1);
	    }
	    else
	    {
	      enriched_putwc ((wchar_t) ' ', &stte);
	      state = NEWLINE;
	    }
	    break;

	  default:
	    enriched_putwc (wc, &stte);
	}
	break;

      case LANGLE :
	if (wc == (wchar_t) '<')
	{
	  enriched_putwc (wc, &stte);
	  state = TEXT;
	  break;
	}
	else
	{
	  tag_len = 0;
	  state = TAG;
	}
        /* fall through */
	/* it wasn't a <<, so this char is first in TAG */
      case TAG :
	if (wc == (wchar_t) '>')
	{
	  tag[tag_len] = (wchar_t) '\0';
	  enriched_set_flags (tag, &stte);
	  state = TEXT;
	}
	else if (tag_len < LONG_STRING)  /* ignore overly long tags */
	  tag[tag_len++] = wc;
	else
	  state = BOGUS_TAG;
	break;

      case BOGUS_TAG :
	if (wc == (wchar_t) '>')
	  state = TEXT;
	break;

      case NEWLINE :
	if (wc == (wchar_t) '\n')
	  enriched_flush (&stte, 1);
	else
	{
	  ungetwc (wc, s->fpin);
	  bytes++;
	  state = TEXT;
	}
	break;

      case ST_EOF :
	enriched_putwc ((wchar_t) '\0', &stte);
        enriched_flush (&stte, 1);
	state = DONE;
	break;

      case DONE: /* not reached, but gcc complains if this is absent */
	break;
    }
  }

  state_putc ('\n', s); /* add a final newline */

  FREE (&(stte.buffer));
  FREE (&(stte.line));
  FREE (&(stte.param));

  return 0;
}