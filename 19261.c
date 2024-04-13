void LinkResolver::resolve_dynamic_call(CallInfo& result,
                                        BootstrapInfo& bootstrap_specifier,
                                        TRAPS) {
  // JSR 292:  this must resolve to an implicitly generated method
  // such as MH.linkToCallSite(*...) or some other call-site shape.
  // The appendix argument is likely to be a freshly-created CallSite.
  // It may also be a MethodHandle from an unwrapped ConstantCallSite,
  // or any other reference.  The resolved_method as well as the appendix
  // are both recorded together via CallInfo::set_handle.
  SystemDictionary::invoke_bootstrap_method(bootstrap_specifier, THREAD);
  Exceptions::wrap_dynamic_exception(/* is_indy */ true, THREAD);

  if (HAS_PENDING_EXCEPTION) {
    if (!PENDING_EXCEPTION->is_a(vmClasses::LinkageError_klass())) {
      // Let any random low-level IE or SOE or OOME just bleed through.
      // Basically we pretend that the bootstrap method was never called,
      // if it fails this way:  We neither record a successful linkage,
      // nor do we memorize a LE for posterity.
      return;
    }
    // JVMS 5.4.3 says: If an attempt by the Java Virtual Machine to resolve
    // a symbolic reference fails because an error is thrown that is an
    // instance of LinkageError (or a subclass), then subsequent attempts to
    // resolve the reference always fail with the same error that was thrown
    // as a result of the initial resolution attempt.
     bool recorded_res_status = bootstrap_specifier.save_and_throw_indy_exc(CHECK);
     if (!recorded_res_status) {
       // Another thread got here just before we did.  So, either use the method
       // that it resolved or throw the LinkageError exception that it threw.
       bool is_done = bootstrap_specifier.resolve_previously_linked_invokedynamic(result, CHECK);
       if (is_done) return;
     }
     assert(bootstrap_specifier.invokedynamic_cp_cache_entry()->indy_resolution_failed(),
            "Resolution failure flag wasn't set");
  }

  bootstrap_specifier.resolve_newly_linked_invokedynamic(result, CHECK);
  // Exceptions::wrap_dynamic_exception not used because
  // set_handle doesn't throw linkage errors
}