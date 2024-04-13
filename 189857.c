static void imap_update_context (IMAP_DATA *idata, int oldmsgcount)
{
  CONTEXT *ctx;
  HEADER *h;
  int msgno;

  ctx = idata->ctx;
  if (!idata->uid_hash)
    idata->uid_hash = int_hash_create (MAX (6 * ctx->msgcount / 5, 30), 0);

  for (msgno = oldmsgcount; msgno < ctx->msgcount; msgno++)
  {
    h = ctx->hdrs[msgno];
    int_hash_insert (idata->uid_hash, HEADER_DATA(h)->uid, h);
  }
}