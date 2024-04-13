Method* LinkResolver::resolve_interface_call_or_null(
                                                 Klass* receiver_klass,
                                                 const LinkInfo& link_info) {
  EXCEPTION_MARK;
  CallInfo info;
  resolve_interface_call(info, Handle(), receiver_klass, link_info, false, THREAD);
  if (HAS_PENDING_EXCEPTION) {
    CLEAR_PENDING_EXCEPTION;
    return NULL;
  }
  return info.selected_method();
}