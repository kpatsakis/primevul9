Method* InstanceKlass::class_initializer() const {
  Method* clinit = find_method(
      vmSymbols::class_initializer_name(), vmSymbols::void_method_signature());
  if (clinit != NULL && clinit->has_valid_initializer_flags()) {
    return clinit;
  }
  return NULL;
}