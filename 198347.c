static void mutt_decode_quoted (STATE *s, LOFF_T len, int istext, iconv_t cd)
{
  char line[STRING];
  char decline[2*STRING];
  size_t l = 0;
  size_t linelen;      /* number of input bytes in `line' */
  size_t l3;

  int last;    /* store the last character in the input line */

  if (istext)
    state_set_prefix(s);

  while (len > 0)
  {
    last = 0;

    /*
     * It's ok to use a fixed size buffer for input, even if the line turns
     * out to be longer than this.  Just process the line in chunks.  This
     * really shouldn't happen according the MIME spec, since Q-P encoded
     * lines are at most 76 characters, but we should be liberal about what
     * we accept.
     */
    if (fgets (line, MIN ((ssize_t)sizeof (line), len + 1), s->fpin) == NULL)
      break;

    linelen = strlen(line);
    len -= linelen;

    /*
     * inspect the last character we read so we can tell if we got the
     * entire line.
     */
    last = linelen ? line[linelen - 1] : 0;

    /* chop trailing whitespace if we got the full line */
    if (last == '\n')
    {
      while (linelen > 0 && ISSPACE (line[linelen-1]))
        linelen--;
      line[linelen]=0;
    }

    /* decode and do character set conversion */
    qp_decode_line (decline + l, line, &l3, last);
    l += l3;
    mutt_convert_to_state (cd, decline, &l, s);
  }

  mutt_convert_to_state (cd, 0, 0, s);
  state_reset_prefix(s);
}