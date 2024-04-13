dirserv_cache_measured_bw(const measured_bw_line_t *parsed_line,
                          time_t as_of)
{
  mbw_cache_entry_t *e = NULL;

  tor_assert(parsed_line);

  /* Allocate a cache if we need */
  if (!mbw_cache) mbw_cache = digestmap_new();

  /* Check if we have an existing entry */
  e = digestmap_get(mbw_cache, parsed_line->node_id);
  /* If we do, we can re-use it */
  if (e) {
    /* Check that we really are newer, and update */
    if (as_of > e->as_of) {
      e->mbw_kb = parsed_line->bw_kb;
      e->as_of = as_of;
    }
  } else {
    /* We'll have to insert a new entry */
    e = tor_malloc(sizeof(*e));
    e->mbw_kb = parsed_line->bw_kb;
    e->as_of = as_of;
    digestmap_set(mbw_cache, parsed_line->node_id, e);
  }
}