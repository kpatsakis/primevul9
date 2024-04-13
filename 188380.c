char *imap_hcache_get_uid_seqset (IMAP_DATA *idata)
{
  char *hc_seqset, *seqset;

  if (!idata->hcache)
    return NULL;

  hc_seqset = mutt_hcache_fetch_raw (idata->hcache, "/UIDSEQSET",
                                     imap_hcache_keylen);
  seqset = safe_strdup (hc_seqset);
  mutt_hcache_free ((void **)&hc_seqset);
  dprint (5, (debugfile, "Retrieved /UIDSEQSET %s\n", NONULL (seqset)));

  return seqset;
}