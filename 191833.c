dirserv_free_all(void)
{
  dirserv_free_fingerprint_list();

  strmap_free(cached_consensuses, free_cached_dir_);
  cached_consensuses = NULL;

  dirserv_clear_measured_bw_cache();
}