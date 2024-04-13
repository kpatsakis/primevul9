void InstanceKlass::rewrite_class(TRAPS) {
  assert(is_loaded(), "must be loaded");
  if (is_rewritten()) {
    assert(is_shared(), "rewriting an unshared class?");
    return;
  }
  Rewriter::rewrite(this, CHECK);
  set_rewritten();
}