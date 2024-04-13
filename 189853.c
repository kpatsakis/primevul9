int imap_close_message (CONTEXT *ctx, MESSAGE *msg)
{
  return safe_fclose (&msg->fp);
}