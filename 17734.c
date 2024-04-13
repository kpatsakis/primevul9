Klass* InstanceKlass::implementor() const {
  assert_locked_or_safepoint(Compile_lock);
  Klass** k = adr_implementor();
  if (k == NULL) {
    return NULL;
  } else {
    return *k;
  }
}