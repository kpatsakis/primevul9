dirserv_get_measured_bw_cache_size(void)
{
  if (mbw_cache) return digestmap_size(mbw_cache);
  else return 0;
}