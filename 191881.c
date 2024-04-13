guardfraction_line_apply(const char *guard_id,
                      uint32_t guardfraction_percentage,
                      smartlist_t *vote_routerstatuses)
{
  vote_routerstatus_t *vrs = NULL;

  tor_assert(vote_routerstatuses);

  vrs = smartlist_bsearch(vote_routerstatuses, guard_id,
                         compare_digest_to_vote_routerstatus_entry);

  if (!vrs) {
    return 0;
  }

  vrs->status.has_guardfraction = 1;
  vrs->status.guardfraction_percentage = guardfraction_percentage;

  return 1;
}