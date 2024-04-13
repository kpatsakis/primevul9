nmethod* InstanceKlass::lookup_osr_nmethod(const Method* m, int bci, int comp_level, bool match_level) const {
  // This is a short non-blocking critical region, so the no safepoint check is ok.
  MutexLockerEx ml(OsrList_lock, Mutex::_no_safepoint_check_flag);
  nmethod* osr = osr_nmethods_head();
  nmethod* best = NULL;
  while (osr != NULL) {
    assert(osr->is_osr_method(), "wrong kind of nmethod found in chain");
    // There can be a time when a c1 osr method exists but we are waiting
    // for a c2 version. When c2 completes its osr nmethod we will trash
    // the c1 version and only be able to find the c2 version. However
    // while we overflow in the c1 code at back branches we don't want to
    // try and switch to the same code as we are already running

    if (osr->method() == m &&
        (bci == InvocationEntryBci || osr->osr_entry_bci() == bci)) {
      if (match_level) {
        if (osr->comp_level() == comp_level) {
          // Found a match - return it.
          return osr;
        }
      } else {
        if (best == NULL || (osr->comp_level() > best->comp_level())) {
          if (osr->comp_level() == CompLevel_highest_tier) {
            // Found the best possible - return it.
            return osr;
          }
          best = osr;
        }
      }
    }
    osr = osr->osr_link();
  }

  assert(match_level == false || best == NULL, "shouldn't pick up anything if match_level is set");
  if (best != NULL && best->comp_level() >= comp_level) {
    return best;
  }
  return NULL;
}