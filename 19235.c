void CallInfo::set_static(Klass* resolved_klass, const methodHandle& resolved_method, TRAPS) {
  int vtable_index = Method::nonvirtual_vtable_index;
  set_common(resolved_klass, resolved_method, resolved_method, CallInfo::direct_call, vtable_index, CHECK);
}