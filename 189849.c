int imap_commit_message (CONTEXT *ctx, MESSAGE *msg)
{
  int r = safe_fclose (&msg->fp);

  if (r)
    return r;

  return imap_append_message (ctx, msg);
}