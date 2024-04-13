Method* LinkResolver::linktime_resolve_interface_method(const LinkInfo& link_info,
                                                             TRAPS) {
  // normal interface method resolution
  Method* resolved_method = resolve_interface_method(link_info, Bytecodes::_invokeinterface, CHECK_NULL);
  assert(resolved_method->name() != vmSymbols::object_initializer_name(), "should have been checked in verifier");
  assert(resolved_method->name() != vmSymbols::class_initializer_name (), "should have been checked in verifier");

  return resolved_method;
}