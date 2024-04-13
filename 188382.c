int imap_hcache_store_uid_seqset (IMAP_DATA *idata)
{
  BUFFER *b;
  int rc;

  if (!idata->hcache)
    return -1;

  b = mutt_buffer_new ();
  /* The seqset is likely large.  Preallocate to reduce reallocs */
  mutt_buffer_increase_size (b, HUGE_STRING);
  imap_msn_index_to_uid_seqset (b, idata);

  rc = mutt_hcache_store_raw (idata->hcache, "/UIDSEQSET",
                              b->data, mutt_buffer_len (b) + 1,
                              imap_hcache_keylen);
  dprint (5, (debugfile, "Stored /UIDSEQSET %s\n", b->data));
  mutt_buffer_free (&b);
  return rc;
}