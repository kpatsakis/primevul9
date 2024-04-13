Method* LinkResolver::lookup_instance_method_in_klasses(Klass* klass,
                                                        Symbol* name,
                                                        Symbol* signature,
                                                        Klass::PrivateLookupMode private_mode) {
  Method* result = klass->uncached_lookup_method(name, signature, Klass::OverpassLookupMode::find, private_mode);

  while (result != NULL && result->is_static() && result->method_holder()->super() != NULL) {
    Klass* super_klass = result->method_holder()->super();
    result = super_klass->uncached_lookup_method(name, signature, Klass::OverpassLookupMode::find, private_mode);
  }

  if (klass->is_array_klass()) {
    // Only consider klass and super klass for arrays
    return result;
  }

  if (result == NULL) {
    Array<Method*>* default_methods = InstanceKlass::cast(klass)->default_methods();
    if (default_methods != NULL) {
      result = InstanceKlass::find_method(default_methods, name, signature);
      assert(result == NULL || !result->is_static(), "static defaults not allowed");
    }
  }
  return result;
}