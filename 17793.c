Method* InstanceKlass::uncached_lookup_method(const Symbol* name,
                                              const Symbol* signature,
                                              OverpassLookupMode overpass_mode,
                                              PrivateLookupMode private_mode) const {
  OverpassLookupMode overpass_local_mode = overpass_mode;
  const Klass* klass = this;
  while (klass != NULL) {
    Method* const method = InstanceKlass::cast(klass)->find_method_impl(name,
                                                                        signature,
                                                                        overpass_local_mode,
                                                                        find_static,
                                                                        private_mode);
    if (method != NULL) {
      return method;
    }
    klass = klass->super();
    overpass_local_mode = skip_overpass;   // Always ignore overpass methods in superclasses
  }
  return NULL;
}