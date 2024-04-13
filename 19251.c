void CallInfo::set_virtual(Klass* resolved_klass,
                           const methodHandle& resolved_method,
                           const methodHandle& selected_method,
                           int vtable_index, TRAPS) {
  assert(vtable_index >= 0 || vtable_index == Method::nonvirtual_vtable_index, "valid index");
  assert(vtable_index < 0 || !resolved_method->has_vtable_index() || vtable_index == resolved_method->vtable_index(), "");
  CallKind kind = (vtable_index >= 0 && !resolved_method->can_be_statically_bound() ? CallInfo::vtable_call : CallInfo::direct_call);
  set_common(resolved_klass, resolved_method, selected_method, kind, vtable_index, CHECK);
  assert(!resolved_method->is_compiled_lambda_form(), "these must be handled via an invokehandle call");
}