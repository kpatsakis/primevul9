bool InstanceKlass::verify_itable_index(int i) {
  int method_count = klassItable::method_count_for_interface(this);
  assert(i >= 0 && i < method_count, "index out of bounds");
  return true;
}