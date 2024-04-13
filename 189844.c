static FILE* msg_cache_get (IMAP_DATA* idata, HEADER* h)
{
  char id[_POSIX_PATH_MAX];

  if (!idata || !h)
    return NULL;

  idata->bcache = msg_cache_open (idata);
  snprintf (id, sizeof (id), "%u-%u", idata->uid_validity, HEADER_DATA(h)->uid);
  return mutt_bcache_get (idata->bcache, id);
}