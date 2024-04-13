bool InstanceKlass::implements_interface(Klass* k) const {
  if (this == k) return true;
  assert(k->is_interface(), "should be an interface class");
  for (int i = 0; i < transitive_interfaces()->length(); i++) {
    if (transitive_interfaces()->at(i) == k) {
      return true;
    }
  }
  return false;
}