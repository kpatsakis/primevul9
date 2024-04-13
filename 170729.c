Method* InstanceKlass::lookup_method_in_ordered_interfaces(Symbol* name,
                                                         Symbol* signature) const {
  Method* m = NULL;
  if (default_methods() != NULL) {
    m = find_method(default_methods(), name, signature);
  }
  // Look up interfaces
  if (m == NULL) {
    m = lookup_method_in_all_interfaces(name, signature, DefaultsLookupMode::find);
  }
  return m;
}