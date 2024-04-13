bool InstanceKlass::compute_is_subtype_of(Klass* k) {
  if (k->is_interface()) {
    return implements_interface(k);
  } else {
    return Klass::compute_is_subtype_of(k);
  }
}