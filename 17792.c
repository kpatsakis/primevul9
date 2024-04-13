bool InstanceKlass::is_same_or_direct_interface(Klass *k) const {
  // Verify direct super interface
  if (this == k) return true;
  assert(k->is_interface(), "should be an interface class");
  for (int i = 0; i < local_interfaces()->length(); i++) {
    if (local_interfaces()->at(i) == k) {
      return true;
    }
  }
  return false;
}