int InstanceKlass::mark_osr_nmethods(const Method* m) {
  MutexLocker ml(CompiledMethod_lock->owned_by_self() ? NULL : CompiledMethod_lock,
                 Mutex::_no_safepoint_check_flag);
  nmethod* osr = osr_nmethods_head();
  int found = 0;
  while (osr != NULL) {
    assert(osr->is_osr_method(), "wrong kind of nmethod found in chain");
    if (osr->method() == m) {
      osr->mark_for_deoptimization();
      found++;
    }
    osr = osr->osr_link();
  }
  return found;
}