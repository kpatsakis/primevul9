Method* LinkResolver::lookup_method_in_interfaces(const LinkInfo& cp_info) {
  InstanceKlass *ik = InstanceKlass::cast(cp_info.resolved_klass());

  // Specify 'true' in order to skip default methods when searching the
  // interfaces.  Function lookup_method_in_klasses() already looked for
  // the method in the default methods table.
  return ik->lookup_method_in_all_interfaces(cp_info.name(), cp_info.signature(), Klass::DefaultsLookupMode::skip);
}