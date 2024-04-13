void LinkResolver::resolve_invokedynamic(CallInfo& result, const constantPoolHandle& pool, int indy_index, TRAPS) {
  ConstantPoolCacheEntry* cpce = pool->invokedynamic_cp_cache_entry_at(indy_index);
  int pool_index = cpce->constant_pool_index();

  // Resolve the bootstrap specifier (BSM + optional arguments).
  BootstrapInfo bootstrap_specifier(pool, pool_index, indy_index);

  // Check if CallSite has been bound already or failed already, and short circuit:
  {
    bool is_done = bootstrap_specifier.resolve_previously_linked_invokedynamic(result, CHECK);
    if (is_done) return;
  }

  // The initial step in Call Site Specifier Resolution is to resolve the symbolic
  // reference to a method handle which will be the bootstrap method for a dynamic
  // call site.  If resolution for the java.lang.invoke.MethodHandle for the bootstrap
  // method fails, then a MethodHandleInError is stored at the corresponding bootstrap
  // method's CP index for the CONSTANT_MethodHandle_info.  So, there is no need to
  // set the indy_rf flag since any subsequent invokedynamic instruction which shares
  // this bootstrap method will encounter the resolution of MethodHandleInError.

  resolve_dynamic_call(result, bootstrap_specifier, CHECK);

  LogTarget(Debug, methodhandles, indy) lt_indy;
  if (lt_indy.is_enabled()) {
    LogStream ls(lt_indy);
    bootstrap_specifier.print_msg_on(&ls, "resolve_invokedynamic");
  }

  // The returned linkage result is provisional up to the moment
  // the interpreter or runtime performs a serialized check of
  // the relevant CPCE::f1 field.  This is done by the caller
  // of this method, via CPCE::set_dynamic_call, which uses
  // an ObjectLocker to do the final serialization of updates
  // to CPCE state, including f1.

  // Log dynamic info to CDS classlist.
  ArchiveUtils::log_to_classlist(&bootstrap_specifier, CHECK);
}