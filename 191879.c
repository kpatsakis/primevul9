dirserv_query_measured_bw_cache_kb(const char *node_id, long *bw_kb_out,
                                   time_t *as_of_out)
{
  mbw_cache_entry_t *v = NULL;
  int rv = 0;

  if (mbw_cache && node_id) {
    v = digestmap_get(mbw_cache, node_id);
    if (v) {
      /* Found something */
      rv = 1;
      if (bw_kb_out) *bw_kb_out = v->mbw_kb;
      if (as_of_out) *as_of_out = v->as_of;
    }
  }

  return rv;
}