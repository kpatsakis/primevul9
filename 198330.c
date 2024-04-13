static void mutt_decode_xbit (STATE *s, LOFF_T len, int istext, iconv_t cd)
{
  int c, ch;
  char bufi[BUFI_SIZE];
  size_t l = 0;

  if (istext)
  {
    state_set_prefix(s);

    while ((c = fgetc(s->fpin)) != EOF && len--)
    {
      if (c == '\r' && len)
      {
	if ((ch = fgetc(s->fpin)) == '\n')
	{
	  c = ch;
	  len--;
	}
	else
	  ungetc(ch, s->fpin);
      }

      bufi[l++] = c;
      if (l == sizeof (bufi))
	mutt_convert_to_state (cd, bufi, &l, s);
    }

    mutt_convert_to_state (cd, bufi, &l, s);
    mutt_convert_to_state (cd, 0, 0, s);

    state_reset_prefix (s);
  }
  else
    mutt_copy_bytes (s->fpin, s->fpout, len);
}