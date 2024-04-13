bool InstanceKlass::remove_osr_nmethod(nmethod* n) {
  // This is a short non-blocking critical region, so the no safepoint check is ok.
  MutexLocker ml(CompiledMethod_lock->owned_by_self() ? NULL : CompiledMethod_lock
                 , Mutex::_no_safepoint_check_flag);
  assert(n->is_osr_method(), "wrong kind of nmethod");
  nmethod* last = NULL;
  nmethod* cur  = osr_nmethods_head();
  int max_level = CompLevel_none;  // Find the max comp level excluding n
  Method* m = n->method();
  // Search for match
  bool found = false;
  while(cur != NULL && cur != n) {
    if (m == cur->method()) {
      // Find max level before n
      max_level = MAX2(max_level, cur->comp_level());
    }
    last = cur;
    cur = cur->osr_link();
  }
  nmethod* next = NULL;
  if (cur == n) {
    found = true;
    next = cur->osr_link();
    if (last == NULL) {
      // Remove first element
      set_osr_nmethods_head(next);
    } else {
      last->set_osr_link(next);
    }
  }
  n->set_osr_link(NULL);
  cur = next;
  while (cur != NULL) {
    // Find max level after n
    if (m == cur->method()) {
      max_level = MAX2(max_level, cur->comp_level());
    }
    cur = cur->osr_link();
  }
  m->set_highest_osr_comp_level(max_level);
  return found;
}