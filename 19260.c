void LinkResolver::resolve_invokehandle(CallInfo& result, const constantPoolHandle& pool, int index, TRAPS) {
  LinkInfo link_info(pool, index, CHECK);
  if (log_is_enabled(Info, methodhandles)) {
    ResourceMark rm(THREAD);
    log_info(methodhandles)("resolve_invokehandle %s %s", link_info.name()->as_C_string(),
                            link_info.signature()->as_C_string());
  }
  { // Check if the call site has been bound already, and short circuit:
    bool is_done = resolve_previously_linked_invokehandle(result, link_info, pool, index, CHECK);
    if (is_done) return;
  }
  resolve_handle_call(result, link_info, CHECK);
}