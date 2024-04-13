Method* LinkResolver::linktime_resolve_virtual_method_or_null(
                                                 const LinkInfo& link_info) {
  EXCEPTION_MARK;
  Method* method_result = linktime_resolve_virtual_method(link_info, THREAD);
  if (HAS_PENDING_EXCEPTION) {
    CLEAR_PENDING_EXCEPTION;
    return NULL;
  } else {
    return method_result;
  }
}