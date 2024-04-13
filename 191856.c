dirserv_remove_old_statuses(smartlist_t *fps, time_t cutoff)
{
  int found_any = 0;
  SMARTLIST_FOREACH_BEGIN(fps, char *, digest) {
    cached_dir_t *d = lookup_cached_dir_by_fp(digest);
    if (!d)
      continue;
    found_any = 1;
    if (d->published <= cutoff) {
      tor_free(digest);
      SMARTLIST_DEL_CURRENT(fps, digest);
    }
  } SMARTLIST_FOREACH_END(digest);

  return found_any;
}