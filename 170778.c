void InstanceKlass::add_osr_nmethod(nmethod* n) {
  assert_lock_strong(CompiledMethod_lock);
#ifndef PRODUCT
  nmethod* prev = lookup_osr_nmethod(n->method(), n->osr_entry_bci(), n->comp_level(), true);
  assert(prev == NULL || !prev->is_in_use() COMPILER2_PRESENT(|| StressRecompilation),
      "redundant OSR recompilation detected. memory leak in CodeCache!");
#endif
  // only one compilation can be active
  assert(n->is_osr_method(), "wrong kind of nmethod");
  n->set_osr_link(osr_nmethods_head());
  set_osr_nmethods_head(n);
  // Raise the highest osr level if necessary
  n->method()->set_highest_osr_comp_level(MAX2(n->method()->highest_osr_comp_level(), n->comp_level()));

  // Get rid of the osr methods for the same bci that have lower levels.
  for (int l = CompLevel_limited_profile; l < n->comp_level(); l++) {
    nmethod *inv = lookup_osr_nmethod(n->method(), n->osr_entry_bci(), l, true);
    if (inv != NULL && inv->is_in_use()) {
      inv->make_not_entrant();
    }
  }
}