dirserv_expire_measured_bw_cache(time_t now)
{

  if (mbw_cache) {
    /* Iterate through the cache and check each entry */
    DIGESTMAP_FOREACH_MODIFY(mbw_cache, k, mbw_cache_entry_t *, e) {
      if (now > e->as_of + MAX_MEASUREMENT_AGE) {
        tor_free(e);
        MAP_DEL_CURRENT(k);
      }
    } DIGESTMAP_FOREACH_END;

    /* Check if we cleared the whole thing and free if so */
    if (digestmap_size(mbw_cache) == 0) {
      digestmap_free(mbw_cache, tor_free_);
      mbw_cache = 0;
    }
  }
}