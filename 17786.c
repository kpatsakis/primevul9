Method* InstanceKlass::find_local_method(const Symbol* name,
                                         const Symbol* signature,
                                         OverpassLookupMode overpass_mode,
                                         StaticLookupMode static_mode,
                                         PrivateLookupMode private_mode) const {
  return InstanceKlass::find_method_impl(methods(),
                                         name,
                                         signature,
                                         overpass_mode,
                                         static_mode,
                                         private_mode);
}