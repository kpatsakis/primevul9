void CallInfo::set_handle(Klass* resolved_klass,
                          const methodHandle& resolved_method,
                          Handle resolved_appendix, TRAPS) {
  guarantee(resolved_method.not_null(), "resolved method is null");
  assert(resolved_method->intrinsic_id() == vmIntrinsics::_invokeBasic ||
         resolved_method->is_compiled_lambda_form(),
         "linkMethod must return one of these");
  int vtable_index = Method::nonvirtual_vtable_index;
  assert(!resolved_method->has_vtable_index(), "");
  set_common(resolved_klass, resolved_method, resolved_method, CallInfo::direct_call, vtable_index, CHECK);
  _resolved_appendix = resolved_appendix;
}