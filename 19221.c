void LinkResolver::runtime_resolve_interface_method(CallInfo& result,
                                                    const methodHandle& resolved_method,
                                                    Klass* resolved_klass,
                                                    Handle recv,
                                                    Klass* recv_klass,
                                                    bool check_null_and_abstract, TRAPS) {

  // check if receiver exists
  if (check_null_and_abstract && recv.is_null()) {
    THROW(vmSymbols::java_lang_NullPointerException());
  }

  // check if receiver klass implements the resolved interface
  if (!recv_klass->is_subtype_of(resolved_klass)) {
    ResourceMark rm(THREAD);
    char buf[200];
    jio_snprintf(buf, sizeof(buf), "Class %s does not implement the requested interface %s",
                 recv_klass->external_name(),
                 resolved_klass->external_name());
    THROW_MSG(vmSymbols::java_lang_IncompatibleClassChangeError(), buf);
  }

  methodHandle selected_method = resolved_method;

  // resolve the method in the receiver class, unless it is private
  if (!resolved_method()->is_private()) {
    // do lookup based on receiver klass
    // This search must match the linktime preparation search for itable initialization
    // to correctly enforce loader constraints for interface method inheritance.
    // Private methods are skipped as the resolved method was not private.
    Method* method = lookup_instance_method_in_klasses(recv_klass,
                                                       resolved_method->name(),
                                                       resolved_method->signature(),
                                                       Klass::PrivateLookupMode::skip);
    selected_method = methodHandle(THREAD, method);

    if (selected_method.is_null() && !check_null_and_abstract) {
      // In theory this is a harmless placeholder value, but
      // in practice leaving in null affects the nsk default method tests.
      // This needs further study.
      selected_method = resolved_method;
    }
    // check if method exists
    if (selected_method.is_null()) {
      // Pass arguments for generating a verbose error message.
      throw_abstract_method_error(resolved_method, recv_klass, CHECK);
    }
    // check access
    // Throw Illegal Access Error if selected_method is not public.
    if (!selected_method->is_public()) {
      ResourceMark rm(THREAD);
      stringStream ss;
      ss.print("'");
      Method::print_external_name(&ss, recv_klass, selected_method->name(), selected_method->signature());
      ss.print("'");
      THROW_MSG(vmSymbols::java_lang_IllegalAccessError(), ss.as_string());
    }
    // check if abstract
    if (check_null_and_abstract && selected_method->is_abstract()) {
      throw_abstract_method_error(resolved_method, selected_method, recv_klass, CHECK);
    }
  }

  if (log_develop_is_enabled(Trace, itables)) {
    trace_method_resolution("invokeinterface selected method: receiver-class:",
                            recv_klass, resolved_klass, selected_method(), true);
  }
  // setup result
  if (resolved_method->has_vtable_index()) {
    int vtable_index = resolved_method->vtable_index();
    log_develop_trace(itables)("  -- vtable index: %d", vtable_index);
    assert(vtable_index == selected_method->vtable_index(), "sanity check");
    result.set_virtual(resolved_klass, resolved_method, selected_method, vtable_index, CHECK);
  } else if (resolved_method->has_itable_index()) {
    int itable_index = resolved_method()->itable_index();
    log_develop_trace(itables)("  -- itable index: %d", itable_index);
    result.set_interface(resolved_klass, resolved_method, selected_method, itable_index, CHECK);
  } else {
    int index = resolved_method->vtable_index();
    log_develop_trace(itables)("  -- non itable/vtable index: %d", index);
    assert(index == Method::nonvirtual_vtable_index, "Oops hit another case!");
    assert(resolved_method()->is_private() ||
           (resolved_method()->is_final() && resolved_method->method_holder() == vmClasses::Object_klass()),
           "Should only have non-virtual invokeinterface for private or final-Object methods!");
    assert(resolved_method()->can_be_statically_bound(), "Should only have non-virtual invokeinterface for statically bound methods!");
    // This sets up the nonvirtual form of "virtual" call (as needed for final and private methods)
    result.set_virtual(resolved_klass, resolved_method, resolved_method, index, CHECK);
  }
}