bool InstanceKlass::link_class_or_fail(TRAPS) {
  assert(is_loaded(), "must be loaded");
  if (!is_linked()) {
    link_class_impl(false, CHECK_false);
  }
  return is_linked();
}