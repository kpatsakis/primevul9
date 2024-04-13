void InstanceKlass::set_implementor(InstanceKlass* ik) {
  assert_locked_or_safepoint(Compile_lock);
  assert(is_interface(), "not interface");
  InstanceKlass* volatile* addr = adr_implementor();
  assert(addr != NULL, "null addr");
  if (addr != NULL) {
    Atomic::release_store(addr, ik);
  }
}