void CallInfo::set_common(Klass* resolved_klass,
                          const methodHandle& resolved_method,
                          const methodHandle& selected_method,
                          CallKind kind,
                          int index,
                          TRAPS) {
  assert(resolved_method->signature() == selected_method->signature(), "signatures must correspond");
  _resolved_klass  = resolved_klass;
  _resolved_method = resolved_method;
  _selected_method = selected_method;
  _call_kind       = kind;
  _call_index      = index;
  _resolved_appendix = Handle();
  DEBUG_ONLY(verify());  // verify before making side effects

  CompilationPolicy::compile_if_required(selected_method, THREAD);
}