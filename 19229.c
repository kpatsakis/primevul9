void LinkResolver::runtime_resolve_virtual_method(CallInfo& result,
                                                  const methodHandle& resolved_method,
                                                  Klass* resolved_klass,
                                                  Handle recv,
                                                  Klass* recv_klass,
                                                  bool check_null_and_abstract,
                                                  TRAPS) {

  // setup default return values
  int vtable_index = Method::invalid_vtable_index;
  methodHandle selected_method;

  // runtime method resolution
  if (check_null_and_abstract && recv.is_null()) { // check if receiver exists
    THROW(vmSymbols::java_lang_NullPointerException());
  }

  // Virtual methods cannot be resolved before its klass has been linked, for otherwise the Method*'s
  // has not been rewritten, and the vtable initialized. Make sure to do this after the nullcheck, since
  // a missing receiver might result in a bogus lookup.
  assert(resolved_method->method_holder()->is_linked(), "must be linked");

  // do lookup based on receiver klass using the vtable index
  if (resolved_method->method_holder()->is_interface()) { // default or miranda method
    vtable_index = vtable_index_of_interface_method(resolved_klass, resolved_method);
    assert(vtable_index >= 0 , "we should have valid vtable index at this point");

    selected_method = methodHandle(THREAD, recv_klass->method_at_vtable(vtable_index));
  } else {
    // at this point we are sure that resolved_method is virtual and not
    // a default or miranda method; therefore, it must have a valid vtable index.
    assert(!resolved_method->has_itable_index(), "");
    vtable_index = resolved_method->vtable_index();
    // We could get a negative vtable_index of nonvirtual_vtable_index for private
    // methods, or for final methods. Private methods never appear in the vtable
    // and never override other methods. As an optimization, final methods are
    // never put in the vtable, unless they override an existing method.
    // So if we do get nonvirtual_vtable_index, it means the selected method is the
    // resolved method, and it can never be changed by an override.
    if (vtable_index == Method::nonvirtual_vtable_index) {
      assert(resolved_method->can_be_statically_bound(), "cannot override this method");
      selected_method = resolved_method;
    } else {
      selected_method = methodHandle(THREAD, recv_klass->method_at_vtable(vtable_index));
    }
  }

  // check if method exists
  if (selected_method.is_null()) {
    throw_abstract_method_error(resolved_method, recv_klass, CHECK);
  }

  // check if abstract
  if (check_null_and_abstract && selected_method->is_abstract()) {
    // Pass arguments for generating a verbose error message.
    throw_abstract_method_error(resolved_method, selected_method, recv_klass, CHECK);
  }

  if (log_develop_is_enabled(Trace, vtables)) {
    trace_method_resolution("invokevirtual selected method: receiver-class:",
                            recv_klass, resolved_klass, selected_method(),
                            false, vtable_index);
  }
  // setup result
  result.set_virtual(resolved_klass, resolved_method, selected_method, vtable_index, CHECK);
}