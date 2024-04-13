dirserv_clear_measured_bw_cache(void)
{
  if (mbw_cache) {
    /* Free the map and all entries */
    digestmap_free(mbw_cache, tor_free_);
    mbw_cache = NULL;
  }
}