dirserv_compute_bridge_flag_thresholds(void)
{
  digestmap_t *omit_as_sybil = digestmap_new();
  dirserv_compute_performance_thresholds(omit_as_sybil);
  digestmap_free(omit_as_sybil, NULL);
}