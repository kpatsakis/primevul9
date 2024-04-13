CallInfo::CallInfo(Method* resolved_method, Klass* resolved_klass, TRAPS) {
  Klass* resolved_method_holder = resolved_method->method_holder();
  if (resolved_klass == NULL) { // 2nd argument defaults to holder of 1st
    resolved_klass = resolved_method_holder;
  }
  _resolved_klass  = resolved_klass;
  _resolved_method = methodHandle(THREAD, resolved_method);
  _selected_method = methodHandle(THREAD, resolved_method);
  // classify:
  CallKind kind = CallInfo::unknown_kind;
  int index = resolved_method->vtable_index();
  if (resolved_method->can_be_statically_bound()) {
    kind = CallInfo::direct_call;
  } else if (!resolved_method_holder->is_interface()) {
    // Could be an Object method inherited into an interface, but still a vtable call.
    kind = CallInfo::vtable_call;
  } else if (!resolved_klass->is_interface()) {
    // A default or miranda method.  Compute the vtable index.
    index = LinkResolver::vtable_index_of_interface_method(resolved_klass, _resolved_method);
    assert(index >= 0 , "we should have valid vtable index at this point");

    kind = CallInfo::vtable_call;
  } else if (resolved_method->has_vtable_index()) {
    // Can occur if an interface redeclares a method of Object.

#ifdef ASSERT
    // Ensure that this is really the case.
    Klass* object_klass = vmClasses::Object_klass();
    Method * object_resolved_method = object_klass->vtable().method_at(index);
    assert(object_resolved_method->name() == resolved_method->name(),
      "Object and interface method names should match at vtable index %d, %s != %s",
      index, object_resolved_method->name()->as_C_string(), resolved_method->name()->as_C_string());
    assert(object_resolved_method->signature() == resolved_method->signature(),
      "Object and interface method signatures should match at vtable index %d, %s != %s",
      index, object_resolved_method->signature()->as_C_string(), resolved_method->signature()->as_C_string());
#endif // ASSERT

    kind = CallInfo::vtable_call;
  } else {
    // A regular interface call.
    kind = CallInfo::itable_call;
    index = resolved_method->itable_index();
  }
  assert(index == Method::nonvirtual_vtable_index || index >= 0, "bad index %d", index);
  _call_kind  = kind;
  _call_index = index;
  _resolved_appendix = Handle();
  // Find or create a ResolvedMethod instance for this Method*
  set_resolved_method_name(CHECK);

  DEBUG_ONLY(verify());
}