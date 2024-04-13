HEADER* imap_hcache_get (IMAP_DATA* idata, unsigned int uid)
{
  char key[16];
  void *data;
  unsigned int uv;
  HEADER* h = NULL;

  if (!idata->hcache)
    return NULL;

  sprintf (key, "/%u", uid);
  data = mutt_hcache_fetch (idata->hcache, key,
                            imap_hcache_keylen);
  if (data)
  {
    memcpy (&uv, data, sizeof(unsigned int));
    if (uv == idata->uid_validity)
      h = mutt_hcache_restore ((unsigned char *)data, NULL);
    else
      dprint (3, (debugfile, "hcache uidvalidity mismatch: %u", uv));
    mutt_hcache_free ((void **)&data);
  }

  return h;
}