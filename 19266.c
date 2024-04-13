Method* LinkResolver::linktime_resolve_special_method(const LinkInfo& link_info, TRAPS) {

  // Invokespecial is called for multiple special reasons:
  // <init>
  // local private method invocation, for classes and interfaces
  // superclass.method, which can also resolve to a default method
  // and the selected method is recalculated relative to the direct superclass
  // superinterface.method, which explicitly does not check shadowing
  Klass* resolved_klass = link_info.resolved_klass();
  Method* resolved_method = NULL;

  if (!resolved_klass->is_interface()) {
    resolved_method = resolve_method(link_info, Bytecodes::_invokespecial, CHECK_NULL);
  } else {
    resolved_method = resolve_interface_method(link_info, Bytecodes::_invokespecial, CHECK_NULL);
  }

  // check if method name is <init>, that it is found in same klass as static type
  if (resolved_method->name() == vmSymbols::object_initializer_name() &&
      resolved_method->method_holder() != resolved_klass) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("%s: method '", resolved_klass->external_name());
    resolved_method->signature()->print_as_signature_external_return_type(&ss);
    ss.print(" %s(", resolved_method->name()->as_C_string());
    resolved_method->signature()->print_as_signature_external_parameters(&ss);
    ss.print(")' not found");
    Exceptions::fthrow(
      THREAD_AND_LOCATION,
      vmSymbols::java_lang_NoSuchMethodError(),
      "%s", ss.as_string());
    return NULL;
  }

  // ensure that invokespecial's interface method reference is in
  // a direct superinterface, not an indirect superinterface
  Klass* current_klass = link_info.current_klass();
  if (current_klass != NULL && resolved_klass->is_interface()) {
    InstanceKlass* klass_to_check = InstanceKlass::cast(current_klass);
    // Disable verification for the dynamically-generated reflection bytecodes.
    bool is_reflect = klass_to_check->is_subclass_of(
                        vmClasses::reflect_MagicAccessorImpl_klass());

    if (!is_reflect &&
        !klass_to_check->is_same_or_direct_interface(resolved_klass)) {
      ResourceMark rm(THREAD);
      stringStream ss;
      ss.print("Interface method reference: '");
      resolved_method->print_external_name(&ss);
      ss.print("', is in an indirect superinterface of %s",
               current_klass->external_name());
      THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
    }
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

  if (log_develop_is_enabled(Trace, itables)) {
    trace_method_resolution("invokespecial resolved method: caller-class:",
                            current_klass, resolved_klass, resolved_method, true);
  }

  return resolved_method;
}