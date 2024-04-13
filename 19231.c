void LinkResolver::resolve_special_call(CallInfo& result,
                                        Handle recv,
                                        const LinkInfo& link_info,
                                        TRAPS) {
  Method* resolved_method = linktime_resolve_special_method(link_info, CHECK);
  runtime_resolve_special_method(result, link_info, methodHandle(THREAD, resolved_method), recv, CHECK);
}