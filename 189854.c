int imap_cache_clean (IMAP_DATA* idata)
{
  idata->bcache = msg_cache_open (idata);
  mutt_bcache_list (idata->bcache, msg_cache_clean_cb, idata);

  return 0;
}