Method* InstanceKlass::find_method(const Symbol* name,
                                   const Symbol* signature) const {
  return find_method_impl(name, signature,
                          OverpassLookupMode::find,
                          StaticLookupMode::find,
                          PrivateLookupMode::find);
}