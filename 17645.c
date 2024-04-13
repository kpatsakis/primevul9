Method* InstanceKlass::find_method_impl(const Array<Method*>* methods,
                                        const Symbol* name,
                                        const Symbol* signature,
                                        OverpassLookupMode overpass_mode,
                                        StaticLookupMode static_mode,
                                        PrivateLookupMode private_mode) {
  int hit = find_method_index(methods, name, signature, overpass_mode, static_mode, private_mode);
  return hit >= 0 ? methods->at(hit): NULL;
}