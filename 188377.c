int imap_hcache_clear_uid_seqset (IMAP_DATA *idata)
{
  if (!idata->hcache)
    return -1;

  return mutt_hcache_delete (idata->hcache, "/UIDSEQSET", imap_hcache_keylen);
}