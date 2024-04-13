read_message_data(FILE *fout)
{
int ch_state;
register int ch;
register int linelength = 0;

/* Handle the case when only EOF terminates the message */

if (!dot_ends)
  {
  register int last_ch = '\n';

  for (; (ch = (receive_getc)(GETC_BUFFER_UNLIMITED)) != EOF; last_ch = ch)
    {
    if (ch == 0) body_zerocount++;
    if (last_ch == '\r' && ch != '\n')
      {
      if (linelength > max_received_linelength)
        max_received_linelength = linelength;
      linelength = 0;
      if (fputc('\n', fout) == EOF) return END_WERROR;
      message_size++;
      body_linecount++;
      }
    if (ch == '\r') continue;

    if (fputc(ch, fout) == EOF) return END_WERROR;
    if (ch == '\n')
      {
      if (linelength > max_received_linelength)
        max_received_linelength = linelength;
      linelength = 0;
      body_linecount++;
      }
    else linelength++;
    if (++message_size > thismessage_size_limit) return END_SIZE;
    }

  if (last_ch != '\n')
    {
    if (linelength > max_received_linelength)
      max_received_linelength = linelength;
    if (fputc('\n', fout) == EOF) return END_WERROR;
    message_size++;
    body_linecount++;
    }

  return END_EOF;
  }

/* Handle the case when a dot on a line on its own, or EOF, terminates. */

ch_state = 1;

while ((ch = (receive_getc)(GETC_BUFFER_UNLIMITED)) != EOF)
  {
  if (ch == 0) body_zerocount++;
  switch (ch_state)
    {
    case 0:                         /* Normal state (previous char written) */
    if (ch == '\n')
      {
      body_linecount++;
      if (linelength > max_received_linelength)
        max_received_linelength = linelength;
      linelength = -1;
      ch_state = 1;
      }
    else if (ch == '\r')
      { ch_state = 2; continue; }
    break;

    case 1:                         /* After written "\n" */
    if (ch == '.') { ch_state = 3; continue; }
    if (ch == '\r') { ch_state = 2; continue; }
    if (ch == '\n') { body_linecount++; linelength = -1; }
    else ch_state = 0;
    break;

    case 2:
    body_linecount++;               /* After unwritten "\r" */
    if (linelength > max_received_linelength)
      max_received_linelength = linelength;
    if (ch == '\n')
      {
      ch_state = 1;
      linelength = -1;
      }
    else
      {
      if (message_size++, fputc('\n', fout) == EOF) return END_WERROR;
      if (ch == '\r') continue;
      ch_state = 0;
      linelength = 0;
      }
    break;

    case 3:                         /* After "\n." (\n written, dot not) */
    if (ch == '\n') return END_DOT;
    if (ch == '\r') { ch_state = 4; continue; }
    message_size++;
    linelength++;
    if (fputc('.', fout) == EOF) return END_WERROR;
    ch_state = 0;
    break;

    case 4:                         /* After "\n.\r" (\n written, rest not) */
    if (ch == '\n') return END_DOT;
    message_size += 2;
    body_linecount++;
    if (fputs(".\n", fout) == EOF) return END_WERROR;
    if (ch == '\r') { ch_state = 2; continue; }
    ch_state = 0;
    break;
    }

  linelength++;
  if (fputc(ch, fout) == EOF) return END_WERROR;
  if (++message_size > thismessage_size_limit) return END_SIZE;
  }

/* Get here if EOF read. Unless we have just written "\n", we need to ensure
the message ends with a newline, and we must also write any characters that
were saved up while testing for an ending dot. */

if (ch_state != 1)
  {
  static uschar *ends[] = { US"\n", NULL, US"\n", US".\n", US".\n" };
  if (fputs(CS ends[ch_state], fout) == EOF) return END_WERROR;
  message_size += Ustrlen(ends[ch_state]);
  body_linecount++;
  }

return END_EOF;
}