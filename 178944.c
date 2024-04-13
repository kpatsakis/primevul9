exit_policy_remove_redundancies(smartlist_t *dest)
{
  addr_policy_t *ap, *tmp, *victim;
  int i, j;

  /* Step one: find a *:* entry and cut off everything after it. */
  for (i = 0; i < smartlist_len(dest); ++i) {
    ap = smartlist_get(dest, i);
    if (ap->maskbits == 0 && ap->prt_min <= 1 && ap->prt_max >= 65535) {
      /* This is a catch-all line -- later lines are unreachable. */
      while (i+1 < smartlist_len(dest)) {
        victim = smartlist_get(dest, i+1);
        smartlist_del(dest, i+1);
        addr_policy_free(victim);
      }
      break;
    }
  }

  /* Step two: for every entry, see if there's a redundant entry
   * later on, and remove it. */
  for (i = 0; i < smartlist_len(dest)-1; ++i) {
    ap = smartlist_get(dest, i);
    for (j = i+1; j < smartlist_len(dest); ++j) {
      tmp = smartlist_get(dest, j);
      tor_assert(j > i);
      if (addr_policy_covers(ap, tmp)) {
        char p1[POLICY_BUF_LEN], p2[POLICY_BUF_LEN];
        policy_write_item(p1, sizeof(p1), tmp, 0);
        policy_write_item(p2, sizeof(p2), ap, 0);
        log(LOG_DEBUG, LD_CONFIG, "Removing exit policy %s (%d).  It is made "
            "redundant by %s (%d).", p1, j, p2, i);
        smartlist_del_keeporder(dest, j--);
        addr_policy_free(tmp);
      }
    }
  }

  /* Step three: for every entry A, see if there's an entry B making this one
   * redundant later on.  This is the case if A and B are of the same type
   * (accept/reject), A is a subset of B, and there is no other entry of
   * different type in between those two that intersects with A.
   *
   * Anybody want to double-check the logic here? XXX
   */
  for (i = 0; i < smartlist_len(dest)-1; ++i) {
    ap = smartlist_get(dest, i);
    for (j = i+1; j < smartlist_len(dest); ++j) {
      // tor_assert(j > i); // j starts out at i+1; j only increases; i only
      //                    // decreases.
      tmp = smartlist_get(dest, j);
      if (ap->policy_type != tmp->policy_type) {
        if (addr_policy_intersects(ap, tmp))
          break;
      } else { /* policy_types are equal. */
        if (addr_policy_covers(tmp, ap)) {
          char p1[POLICY_BUF_LEN], p2[POLICY_BUF_LEN];
          policy_write_item(p1, sizeof(p1), ap, 0);
          policy_write_item(p2, sizeof(p2), tmp, 0);
          log(LOG_DEBUG, LD_CONFIG, "Removing exit policy %s.  It is already "
              "covered by %s.", p1, p2);
          smartlist_del_keeporder(dest, i--);
          addr_policy_free(ap);
          break;
        }
      }
    }
  }
}