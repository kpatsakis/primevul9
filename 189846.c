int imap_append_message (CONTEXT *ctx, MESSAGE *msg)
{
  IMAP_DATA* idata;
  FILE *fp;
  char buf[LONG_STRING];
  char mbox[LONG_STRING];
  char mailbox[LONG_STRING];
  char internaldate[IMAP_DATELEN];
  char imap_flags[SHORT_STRING];
  size_t len;
  progress_t progressbar;
  size_t sent;
  int c, last;
  IMAP_MBOX mx;
  int rc;

  idata = (IMAP_DATA*) ctx->data;

  if (imap_parse_path (ctx->path, &mx))
    return -1;

  imap_fix_path (idata, mx.mbox, mailbox, sizeof (mailbox));
  if (!*mailbox)
    strfcpy (mailbox, "INBOX", sizeof (mailbox));

  if ((fp = fopen (msg->path, "r")) == NULL)
  {
    mutt_perror (msg->path);
    goto fail;
  }

  /* currently we set the \Seen flag on all messages, but probably we
   * should scan the message Status header for flag info. Since we're
   * already rereading the whole file for length it isn't any more
   * expensive (it'd be nice if we had the file size passed in already
   * by the code that writes the file, but that's a lot of changes.
   * Ideally we'd have a HEADER structure with flag info here... */
  for (last = EOF, len = 0; (c = fgetc(fp)) != EOF; last = c)
  {
    if(c == '\n' && last != '\r')
      len++;

    len++;
  }
  rewind (fp);

  mutt_progress_init (&progressbar, _("Uploading message..."),
		      MUTT_PROGRESS_SIZE, NetInc, len);

  imap_munge_mbox_name (idata, mbox, sizeof (mbox), mailbox);
  imap_make_date (internaldate, msg->received);

  imap_flags[0] = imap_flags[1] = 0;
  if (msg->flags.read)
    safe_strcat (imap_flags, sizeof (imap_flags), " \\Seen");
  if (msg->flags.replied)
    safe_strcat (imap_flags, sizeof (imap_flags), " \\Answered");
  if (msg->flags.flagged)
    safe_strcat (imap_flags, sizeof (imap_flags), " \\Flagged");
  if (msg->flags.draft)
    safe_strcat (imap_flags, sizeof (imap_flags), " \\Draft");

  snprintf (buf, sizeof (buf), "APPEND %s (%s) \"%s\" {%lu}", mbox,
            imap_flags + 1,
	    internaldate,
	    (unsigned long) len);

  imap_cmd_start (idata, buf);

  do
    rc = imap_cmd_step (idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (rc != IMAP_CMD_RESPOND)
  {
    char *pc;

    dprint (1, (debugfile, "imap_append_message(): command failed: %s\n",
		idata->buf));

    pc = idata->buf + SEQLEN;
    SKIPWS (pc);
    pc = imap_next_word (pc);
    mutt_error ("%s", pc);
    mutt_sleep (1);
    safe_fclose (&fp);
    goto fail;
  }

  for (last = EOF, sent = len = 0; (c = fgetc(fp)) != EOF; last = c)
  {
    if (c == '\n' && last != '\r')
      buf[len++] = '\r';

    buf[len++] = c;

    if (len > sizeof(buf) - 3)
    {
      sent += len;
      flush_buffer(buf, &len, idata->conn);
      mutt_progress_update (&progressbar, sent, -1);
    }
  }

  if (len)
    flush_buffer(buf, &len, idata->conn);

  mutt_socket_write (idata->conn, "\r\n");
  safe_fclose (&fp);

  do
    rc = imap_cmd_step (idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (!imap_code (idata->buf))
  {
    char *pc;

    dprint (1, (debugfile, "imap_append_message(): command failed: %s\n",
		idata->buf));
    pc = idata->buf + SEQLEN;
    SKIPWS (pc);
    pc = imap_next_word (pc);
    mutt_error ("%s", pc);
    mutt_sleep (1);
    goto fail;
  }

  FREE (&mx.mbox);
  return 0;

 fail:
  FREE (&mx.mbox);
  return -1;
}