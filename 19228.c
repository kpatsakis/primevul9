void LinkResolver::runtime_resolve_special_method(CallInfo& result,
                                                  const LinkInfo& link_info,
                                                  const methodHandle& resolved_method,
                                                  Handle recv, TRAPS) {

  Klass* resolved_klass = link_info.resolved_klass();

  // resolved method is selected method unless we have an old-style lookup
  // for a superclass method
  // Invokespecial for a superinterface, resolved method is selected method,
  // no checks for shadowing
  methodHandle sel_method(THREAD, resolved_method());

  if (link_info.check_access() &&
      // check if the method is not <init>
      resolved_method->name() != vmSymbols::object_initializer_name()) {

    Klass* current_klass = link_info.current_klass();

    // Check if the class of the resolved_klass is a superclass
    // (not supertype in order to exclude interface classes) of the current class.
    // This check is not performed for super.invoke for interface methods
    // in super interfaces.
    if (current_klass->is_subclass_of(resolved_klass) &&
        current_klass != resolved_klass) {
      // Lookup super method
      Klass* super_klass = current_klass->super();
      Method* instance_method = lookup_instance_method_in_klasses(super_klass,
                                                     resolved_method->name(),
                                                     resolved_method->signature(),
                                                     Klass::PrivateLookupMode::find);
      sel_method = methodHandle(THREAD, instance_method);

      // check if found
      if (sel_method.is_null()) {
        ResourceMark rm(THREAD);
        stringStream ss;
        ss.print("'");
        resolved_method->print_external_name(&ss);
        ss.print("'");
        THROW_MSG(vmSymbols::java_lang_AbstractMethodError(), ss.as_string());
      // check loader constraints if found a different method
      } else if (link_info.check_loader_constraints() && sel_method() != resolved_method()) {
        check_method_loader_constraints(link_info, sel_method, "method", CHECK);
      }
    }

    // Check that the class of objectref (the receiver) is the current class or interface,
    // or a subtype of the current class or interface (the sender), otherwise invokespecial
    // throws IllegalAccessError.
    // The verifier checks that the sender is a subtype of the class in the I/MR operand.
    // The verifier also checks that the receiver is a subtype of the sender, if the sender is
    // a class.  If the sender is an interface, the check has to be performed at runtime.
    InstanceKlass* sender = InstanceKlass::cast(current_klass);
    if (sender->is_interface() && recv.not_null()) {
      Klass* receiver_klass = recv->klass();
      if (!receiver_klass->is_subtype_of(sender)) {
        ResourceMark rm(THREAD);
        char buf[500];
        jio_snprintf(buf, sizeof(buf),
                     "Receiver class %s must be the current class or a subtype of interface %s",
                     receiver_klass->external_name(),
                     sender->external_name());
        THROW_MSG(vmSymbols::java_lang_IllegalAccessError(), buf);
      }
    }
  }

  // check if not static
  if (sel_method->is_static()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("Expecting non-static method '");
    resolved_method->print_external_name(&ss);
    ss.print("'");
    THROW_MSG(vmSymbols::java_lang_IncompatibleClassChangeError(), ss.as_string());
  }

  // check if abstract
  if (sel_method->is_abstract()) {
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("'");
    Method::print_external_name(&ss, resolved_klass, sel_method->name(), sel_method->signature());
    ss.print("'");
    THROW_MSG(vmSymbols::java_lang_AbstractMethodError(), ss.as_string());
  }

  if (log_develop_is_enabled(Trace, itables)) {
    trace_method_resolution("invokespecial selected method: resolved-class:",
                            resolved_klass, resolved_klass, sel_method(), true);
  }

  // setup result
  result.set_static(resolved_klass, sel_method, CHECK);
}