void LinkResolver::resolve_virtual_call(CallInfo& result, Handle recv, Klass* receiver_klass,
                                        const LinkInfo& link_info,
                                        bool check_null_and_abstract, TRAPS) {
  Method* resolved_method = linktime_resolve_virtual_method(link_info, CHECK);
  runtime_resolve_virtual_method(result, methodHandle(THREAD, resolved_method),
                                 link_info.resolved_klass(),
                                 recv, receiver_klass,
                                 check_null_and_abstract, CHECK);
}