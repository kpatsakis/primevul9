policy_summary_reject(smartlist_t *summary,
                      maskbits_t maskbits,
                      uint16_t prt_min, uint16_t prt_max)
{
  int i = policy_summary_split(summary, prt_min, prt_max);
  /* XXX: ipv4 specific */
  uint64_t count = (U64_LITERAL(1) << (32-maskbits));
  while (i < smartlist_len(summary) &&
         AT(i)->prt_max <= prt_max) {
    AT(i)->reject_count += count;
    i++;
  }
  tor_assert(i < smartlist_len(summary) || prt_max==65535);
}