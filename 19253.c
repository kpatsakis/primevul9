void CallInfo::set_interface(Klass* resolved_klass,
                             const methodHandle& resolved_method,
                             const methodHandle& selected_method,
                             int itable_index, TRAPS) {
  // This is only called for interface methods. If the resolved_method
  // comes from java/lang/Object, it can be the subject of a virtual call, so
  // we should pick the vtable index from the resolved method.
  // In that case, the caller must call set_virtual instead of set_interface.
  assert(resolved_method->method_holder()->is_interface(), "");
  assert(itable_index == resolved_method()->itable_index(), "");
  set_common(resolved_klass, resolved_method, selected_method, CallInfo::itable_call, itable_index, CHECK);
}