Method* LinkResolver::linktime_resolve_virtual_method(const LinkInfo& link_info,
                                                           TRAPS) {
  // normal method resolution
  Method* resolved_method = resolve_method(link_info, Bytecodes::_invokevirtual, CHECK_NULL);

  assert(resolved_method->name() != vmSymbols::object_initializer_name(), "should have been checked in verifier");
  assert(resolved_method->name() != vmSymbols::class_initializer_name (), "should have been checked in verifier");

  // check if private interface method
  Klass* resolved_klass = link_info.resolved_klass();
  Klass* current_klass = link_info.current_klass();

  // This is impossible, if resolve_klass is an interface, we've thrown icce in resolve_method
  if (resolved_klass->is_interface() && resolved_method->is_private()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("private interface method requires invokespecial, not invokevirtual: method '");
    resolved_method->print_external_name(&ss);
    ss.print("', caller-class: %s",
             (current_klass == NULL ? "<null>" : current_klass->internal_name()));
    THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
  }

  // check if not static
  if (resolved_method->is_static()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("Expecting non-static method '");
    resolved_method->print_external_name(&ss);
    ss.print("'");
    THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
  }

  if (log_develop_is_enabled(Trace, vtables)) {
    trace_method_resolution("invokevirtual resolved method: caller-class:",
                            current_klass, resolved_klass, resolved_method, false);
  }

  return resolved_method;
}