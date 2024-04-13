bool LinkResolver::resolve_previously_linked_invokehandle(CallInfo& result, const LinkInfo& link_info, const constantPoolHandle& pool, int index, TRAPS) {
  int cache_index = ConstantPool::decode_cpcache_index(index, true);
  ConstantPoolCacheEntry* cpce = pool->cache()->entry_at(cache_index);
  if (!cpce->is_f1_null()) {
    Klass* resolved_klass = link_info.resolved_klass();
    methodHandle method(THREAD, cpce->f1_as_method());
    Handle     appendix(THREAD, cpce->appendix_if_resolved(pool));
    result.set_handle(resolved_klass, method, appendix, CHECK_false);
    return true;
  } else {
    return false;
  }
}