void LinkResolver::resolve_interface_call(CallInfo& result, Handle recv, Klass* recv_klass,
                                          const LinkInfo& link_info,
                                          bool check_null_and_abstract, TRAPS) {
  // throws linktime exceptions
  Method* resolved_method = linktime_resolve_interface_method(link_info, CHECK);
  methodHandle mh(THREAD, resolved_method);
  runtime_resolve_interface_method(result, mh, link_info.resolved_klass(),
                                   recv, recv_klass, check_null_and_abstract, CHECK);
}