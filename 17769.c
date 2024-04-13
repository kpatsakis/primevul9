int  InstanceKlass::nof_implementors() const {
  assert_lock_strong(Compile_lock);
  Klass* k = implementor();
  if (k == NULL) {
    return 0;
  } else if (k != this) {
    return 1;
  } else {
    return 2;
  }
}