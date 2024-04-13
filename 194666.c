read_message_bdat_smtp(FILE *fout)
{
int linelength = 0, ch;
enum CH_STATE ch_state = LF_SEEN;
BOOL fix_nl = FALSE;

for(;;)
  {
  switch ((ch = bdat_getc(GETC_BUFFER_UNLIMITED)))
    {
    case EOF:	return END_EOF;
    case ERR:	return END_PROTOCOL;
    case EOD:
      /* Nothing to get from the sender anymore. We check the last
      character written to the spool.

      RFC 3030 states, that BDAT chunks are normal text, terminated by CRLF.
      If we would be strict, we would refuse such broken messages.
      But we are liberal, so we fix it.  It would be easy just to append
      the "\n" to the spool.

      But there are some more things (line counting, message size calculation and such),
      that would need to be duplicated here.  So we simply do some ungetc
      trickery.
      */
      if (fout)
	{
	if (fseek(fout, -1, SEEK_CUR) < 0)	return END_PROTOCOL;
	if (fgetc(fout) == '\n')		return END_DOT;
	}

      if (linelength == -1)    /* \r already seen (see below) */
        {
        DEBUG(D_receive) debug_printf("Add missing LF\n");
        bdat_ungetc('\n');
        continue;
        }
      DEBUG(D_receive) debug_printf("Add missing CRLF\n");
      bdat_ungetc('\r');      /* not even \r was seen */
      fix_nl = TRUE;

      continue;
    case '\0':  body_zerocount++; break;
    }
  switch (ch_state)
    {
    case LF_SEEN:                             /* After LF or CRLF */
      ch_state = MID_LINE;
      /* fall through to handle as normal uschar. */

    case MID_LINE:                            /* Mid-line state */
      if (ch == '\n')
	{
	ch_state = LF_SEEN;
	body_linecount++;
	if (linelength > max_received_linelength)
	  max_received_linelength = linelength;
	linelength = -1;
	}
      else if (ch == '\r')
	{
	ch_state = CR_SEEN;
       if (fix_nl) bdat_ungetc('\n');
	continue;			/* don't write CR */
	}
      break;

    case CR_SEEN:                       /* After (unwritten) CR */
      body_linecount++;
      if (linelength > max_received_linelength)
	max_received_linelength = linelength;
      linelength = -1;
      if (ch == '\n')
	ch_state = LF_SEEN;
      else
	{
	message_size++;
	if (fout && fputc('\n', fout) == EOF) return END_WERROR;
	cutthrough_data_put_nl();
	if (ch == '\r') continue;	/* don't write CR */
	ch_state = MID_LINE;
	}
      break;
    }

  /* Add the character to the spool file, unless skipping */

  message_size++;
  linelength++;
  if (fout)
    {
    if (fputc(ch, fout) == EOF) return END_WERROR;
    if (message_size > thismessage_size_limit) return END_SIZE;
    }
  if(ch == '\n')
    cutthrough_data_put_nl();
  else
    {
    uschar c = ch;
    cutthrough_data_puts(&c, 1);
    }
  }
/*NOTREACHED*/
}