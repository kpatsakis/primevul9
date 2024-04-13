static body_cache_t *msg_cache_open (IMAP_DATA *idata)
{
  char mailbox[_POSIX_PATH_MAX];

  if (idata->bcache)
    return idata->bcache;

  imap_cachepath (idata, idata->mailbox, mailbox, sizeof (mailbox));

  return mutt_bcache_open (&idata->conn->account, mailbox);
}