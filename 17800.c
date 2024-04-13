void InstanceKlass::link_class(TRAPS) {
  assert(is_loaded(), "must be loaded");
  if (!is_linked()) {
    link_class_impl(true, CHECK);
  }
}