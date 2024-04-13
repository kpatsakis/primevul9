Method* LinkResolver::resolve_method_statically(Bytecodes::Code code,
                                                const constantPoolHandle& pool, int index, TRAPS) {
  // This method is used only
  // (1) in C2 from InlineTree::ok_to_inline (via ciMethod::check_call),
  // and
  // (2) in Bytecode_invoke::static_target
  // It appears to fail when applied to an invokeinterface call site.
  // FIXME: Remove this method and ciMethod::check_call; refactor to use the other LinkResolver entry points.
  // resolve klass
  if (code == Bytecodes::_invokedynamic) {
    Klass* resolved_klass = vmClasses::MethodHandle_klass();
    Symbol* method_name = vmSymbols::invoke_name();
    Symbol* method_signature = pool->signature_ref_at(index);
    Klass*  current_klass = pool->pool_holder();
    LinkInfo link_info(resolved_klass, method_name, method_signature, current_klass);
    return resolve_method(link_info, code, THREAD);
  }

  LinkInfo link_info(pool, index, methodHandle(), CHECK_NULL);
  Klass* resolved_klass = link_info.resolved_klass();

  if (pool->has_preresolution()
      || ((resolved_klass == vmClasses::MethodHandle_klass() || resolved_klass == vmClasses::VarHandle_klass()) &&
          MethodHandles::is_signature_polymorphic_name(resolved_klass, link_info.name()))) {
    Method* result = ConstantPool::method_at_if_loaded(pool, index);
    if (result != NULL) {
      return result;
    }
  }

  if (code == Bytecodes::_invokeinterface) {
    return resolve_interface_method(link_info, code, THREAD);
  } else if (code == Bytecodes::_invokevirtual) {
    return resolve_method(link_info, code, THREAD);
  } else if (!resolved_klass->is_interface()) {
    return resolve_method(link_info, code, THREAD);
  } else {
    return resolve_interface_method(link_info, code, THREAD);
  }
}