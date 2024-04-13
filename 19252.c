void LinkResolver::resolve_static_call(CallInfo& result,
                                       const LinkInfo& link_info,
                                       bool initialize_class, TRAPS) {
  Method* resolved_method = linktime_resolve_static_method(link_info, CHECK);

  // The resolved class can change as a result of this resolution.
  Klass* resolved_klass = resolved_method->method_holder();

  // Initialize klass (this should only happen if everything is ok)
  if (initialize_class && resolved_klass->should_be_initialized()) {
    resolved_klass->initialize(CHECK);
    // Use updated LinkInfo to reresolve with resolved method holder
    LinkInfo new_info(resolved_klass, link_info.name(), link_info.signature(),
                      link_info.current_klass(),
                      link_info.check_access() ? LinkInfo::AccessCheck::required : LinkInfo::AccessCheck::skip,
                      link_info.check_loader_constraints() ? LinkInfo::LoaderConstraintCheck::required : LinkInfo::LoaderConstraintCheck::skip);
    resolved_method = linktime_resolve_static_method(new_info, CHECK);
  }

  // setup result
  result.set_static(resolved_klass, methodHandle(THREAD, resolved_method), CHECK);
}