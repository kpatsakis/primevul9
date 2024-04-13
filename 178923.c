policy_summary_accept(smartlist_t *summary,
                      uint16_t prt_min, uint16_t prt_max)
{
  int i = policy_summary_split(summary, prt_min, prt_max);
  while (i < smartlist_len(summary) &&
         AT(i)->prt_max <= prt_max) {
    if (!AT(i)->accepted &&
        AT(i)->reject_count <= REJECT_CUTOFF_COUNT)
      AT(i)->accepted = 1;
    i++;
  }
  tor_assert(i < smartlist_len(summary) || prt_max==65535);
}