void InstanceKlass::set_implementor(Klass* k) {
  assert_lock_strong(Compile_lock);
  assert(is_interface(), "not interface");
  Klass** addr = adr_implementor();
  assert(addr != NULL, "null addr");
  if (addr != NULL) {
    *addr = k;
  }
}