dirserv_estimate_data_size(smartlist_t *fps, int is_serverdescs,
                           int compressed)
{
  size_t result;
  tor_assert(fps);
  if (is_serverdescs) {
    int n = smartlist_len(fps);
    const routerinfo_t *me = router_get_my_routerinfo();
    result = (me?me->cache_info.signed_descriptor_len:2048) * n;
    if (compressed)
      result /= 2; /* observed compressibility is between 35 and 55%. */
  } else {
    result = 0;
    SMARTLIST_FOREACH(fps, const char *, digest, {
        cached_dir_t *dir = lookup_cached_dir_by_fp(digest);
        if (dir)
          result += compressed ? dir->dir_z_len : dir->dir_len;
      });
  }
  return result;
}