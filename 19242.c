void CallInfo::set_resolved_method_name(TRAPS) {
  assert(_resolved_method() != NULL, "Should already have a Method*");
  oop rmethod_name = java_lang_invoke_ResolvedMethodName::find_resolved_method(_resolved_method, CHECK);
  _resolved_method_name = Handle(THREAD, rmethod_name);
}