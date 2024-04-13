static int msg_cache_clean_cb (const char* id, body_cache_t* bcache, void* data)
{
  unsigned int uv, uid;
  IMAP_DATA* idata = (IMAP_DATA*)data;

  if (sscanf (id, "%u-%u", &uv, &uid) != 2)
    return 0;

  /* bad UID */
  if (uv != idata->uid_validity ||
      !int_hash_find (idata->uid_hash, uid))
    mutt_bcache_del (bcache, id);

  return 0;
}