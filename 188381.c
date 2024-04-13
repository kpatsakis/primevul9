header_cache_t* imap_hcache_open (IMAP_DATA* idata, const char* path)
{
  IMAP_MBOX mx;
  ciss_url_t url;
  BUFFER *cachepath = NULL;
  BUFFER *mbox = NULL;
  size_t len;
  header_cache_t *rv = NULL;

  mbox = mutt_buffer_pool_get ();
  cachepath = mutt_buffer_pool_get ();

  if (path)
    imap_cachepath (idata, path, mbox);
  else
  {
    if (!idata->ctx || imap_parse_path (idata->ctx->path, &mx) < 0)
      goto cleanup;

    imap_cachepath (idata, mx.mbox, mbox);
    FREE (&mx.mbox);
  }

  if (strstr (mutt_b2s (mbox), "/../") ||
      (strcmp (mutt_b2s (mbox), "..") == 0) ||
      (strncmp(mutt_b2s (mbox), "../", 3) == 0))
    goto cleanup;
  len = mutt_buffer_len (mbox);
  if ((len > 3) && (strcmp(mutt_b2s (mbox) + len - 3, "/..") == 0))
    goto cleanup;

  mutt_account_tourl (&idata->conn->account, &url);
  url.path = mbox->data;
  url_ciss_tobuffer (&url, cachepath, U_PATH);

  rv = mutt_hcache_open (HeaderCache, mutt_b2s (cachepath), imap_hcache_namer);

cleanup:
  mutt_buffer_pool_release (&mbox);
  mutt_buffer_pool_release (&cachepath);
  return rv;
}