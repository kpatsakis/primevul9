bool InstanceKlass::has_redefined_this_or_super() const {
  const Klass* klass = this;
  while (klass != NULL) {
    if (InstanceKlass::cast(klass)->has_been_redefined()) {
      return true;
    }
    klass = klass->super();
  }
  return false;
}