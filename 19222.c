Method* LinkResolver::linktime_resolve_static_method(const LinkInfo& link_info, TRAPS) {

  Klass* resolved_klass = link_info.resolved_klass();
  Method* resolved_method;
  if (!resolved_klass->is_interface()) {
    resolved_method = resolve_method(link_info, Bytecodes::_invokestatic, CHECK_NULL);
  } else {
    resolved_method = resolve_interface_method(link_info, Bytecodes::_invokestatic, CHECK_NULL);
  }
  assert(resolved_method->name() != vmSymbols::class_initializer_name(), "should have been checked in verifier");

  // check if static
  if (!resolved_method->is_static()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("Expected static method '");
    resolved_method->print_external_name(&ss);
    ss.print("'");
    THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
  }
  return resolved_method;
}