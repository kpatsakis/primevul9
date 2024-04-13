Method* LinkResolver::resolve_method(const LinkInfo& link_info,
                                     Bytecodes::Code code, TRAPS) {

  Handle nested_exception;
  Klass* resolved_klass = link_info.resolved_klass();

  // 1. For invokevirtual, cannot call an interface method
  if (code == Bytecodes::_invokevirtual && resolved_klass->is_interface()) {
    ResourceMark rm(THREAD);
    char buf[200];
    jio_snprintf(buf, sizeof(buf), "Found interface %s, but class was expected",
        resolved_klass->external_name());
    THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), buf);
  }

  // 2. check constant pool tag for called method - must be JVM_CONSTANT_Methodref
  if (!link_info.tag().is_invalid() && !link_info.tag().is_method()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("Method '");
    Method::print_external_name(&ss, link_info.resolved_klass(), link_info.name(), link_info.signature());
    ss.print("' must be Methodref constant");
    THROW_MSG_NULL(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
  }

  // 3. lookup method in resolved klass and its super klasses
  methodHandle resolved_method(THREAD, lookup_method_in_klasses(link_info, true, false));

  // 4. lookup method in all the interfaces implemented by the resolved klass
  if (resolved_method.is_null() && !resolved_klass->is_array_klass()) { // not found in the class hierarchy
    resolved_method = methodHandle(THREAD, lookup_method_in_interfaces(link_info));

    if (resolved_method.is_null()) {
      // JSR 292:  see if this is an implicitly generated method MethodHandle.linkToVirtual(*...), etc
      Method* method = lookup_polymorphic_method(link_info, (Handle*)NULL, THREAD);
      resolved_method = methodHandle(THREAD, method);
      if (HAS_PENDING_EXCEPTION) {
        nested_exception = Handle(THREAD, PENDING_EXCEPTION);
        CLEAR_PENDING_EXCEPTION;
      }
    }
  }

  // 5. method lookup failed
  if (resolved_method.is_null()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("'");
    Method::print_external_name(&ss, resolved_klass, link_info.name(), link_info.signature());
    ss.print("'");
    THROW_MSG_CAUSE_(vmSymbols::java_lang_NoSuchMethodError(),
                     ss.as_string(), nested_exception, NULL);
  }

  // 6. access checks, access checking may be turned off when calling from within the VM.
  Klass* current_klass = link_info.current_klass();
  if (link_info.check_access()) {
    assert(current_klass != NULL , "current_klass should not be null");

    // check if method can be accessed by the referring class
    check_method_accessability(current_klass,
                               resolved_klass,
                               resolved_method->method_holder(),
                               resolved_method,
                               CHECK_NULL);
  }
  if (link_info.check_loader_constraints()) {
    // check loader constraints
    check_method_loader_constraints(link_info, resolved_method, "method", CHECK_NULL);
  }

  return resolved_method();
}