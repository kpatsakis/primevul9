Method* InstanceKlass::find_method(const Array<Method*>* methods,
                                   const Symbol* name,
                                   const Symbol* signature) {
  return InstanceKlass::find_method_impl(methods,
                                         name,
                                         signature,
                                         OverpassLookupMode::find,
                                         StaticLookupMode::find,
                                         PrivateLookupMode::find);
}