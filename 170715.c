bool InstanceKlass::link_class_impl(TRAPS) {
  if (DumpSharedSpaces && SystemDictionaryShared::has_class_failed_verification(this)) {
    // This is for CDS dumping phase only -- we use the in_error_state to indicate that
    // the class has failed verification. Throwing the NoClassDefFoundError here is just
    // a convenient way to stop repeat attempts to verify the same (bad) class.
    //
    // Note that the NoClassDefFoundError is not part of the JLS, and should not be thrown
    // if we are executing Java code. This is not a problem for CDS dumping phase since
    // it doesn't execute any Java code.
    ResourceMark rm(THREAD);
    Exceptions::fthrow(THREAD_AND_LOCATION,
                       vmSymbols::java_lang_NoClassDefFoundError(),
                       "Class %s, or one of its supertypes, failed class initialization",
                       external_name());
    return false;
  }
  // return if already verified
  if (is_linked()) {
    return true;
  }

  // Timing
  // timer handles recursion
  JavaThread* jt = THREAD;

  // link super class before linking this class
  Klass* super_klass = super();
  if (super_klass != NULL) {
    if (super_klass->is_interface()) {  // check if super class is an interface
      ResourceMark rm(THREAD);
      Exceptions::fthrow(
        THREAD_AND_LOCATION,
        vmSymbols::java_lang_IncompatibleClassChangeError(),
        "class %s has interface %s as super class",
        external_name(),
        super_klass->external_name()
      );
      return false;
    }

    InstanceKlass* ik_super = InstanceKlass::cast(super_klass);
    ik_super->link_class_impl(CHECK_false);
  }

  // link all interfaces implemented by this class before linking this class
  Array<InstanceKlass*>* interfaces = local_interfaces();
  int num_interfaces = interfaces->length();
  for (int index = 0; index < num_interfaces; index++) {
    InstanceKlass* interk = interfaces->at(index);
    interk->link_class_impl(CHECK_false);
  }

  // in case the class is linked in the process of linking its superclasses
  if (is_linked()) {
    return true;
  }

  // trace only the link time for this klass that includes
  // the verification time
  PerfClassTraceTime vmtimer(ClassLoader::perf_class_link_time(),
                             ClassLoader::perf_class_link_selftime(),
                             ClassLoader::perf_classes_linked(),
                             jt->get_thread_stat()->perf_recursion_counts_addr(),
                             jt->get_thread_stat()->perf_timers_addr(),
                             PerfClassTraceTime::CLASS_LINK);

  // verification & rewriting
  {
    HandleMark hm(THREAD);
    Handle h_init_lock(THREAD, init_lock());
    ObjectLocker ol(h_init_lock, jt);
    // rewritten will have been set if loader constraint error found
    // on an earlier link attempt
    // don't verify or rewrite if already rewritten
    //

    if (!is_linked()) {
      if (!is_rewritten()) {
        {
          bool verify_ok = verify_code(THREAD);
          if (!verify_ok) {
            return false;
          }
        }

        // Just in case a side-effect of verify linked this class already
        // (which can sometimes happen since the verifier loads classes
        // using custom class loaders, which are free to initialize things)
        if (is_linked()) {
          return true;
        }

        // also sets rewritten
        rewrite_class(CHECK_false);
      } else if (is_shared()) {
        SystemDictionaryShared::check_verification_constraints(this, CHECK_false);
      }

      // relocate jsrs and link methods after they are all rewritten
      link_methods(CHECK_false);

      // Initialize the vtable and interface table after
      // methods have been rewritten since rewrite may
      // fabricate new Method*s.
      // also does loader constraint checking
      //
      // initialize_vtable and initialize_itable need to be rerun
      // for a shared class if
      // 1) the class is loaded by custom class loader or
      // 2) the class is loaded by built-in class loader but failed to add archived loader constraints
      bool need_init_table = true;
      if (is_shared() && SystemDictionaryShared::check_linking_constraints(THREAD, this)) {
        need_init_table = false;
      }
      if (need_init_table) {
        vtable().initialize_vtable_and_check_constraints(CHECK_false);
        itable().initialize_itable_and_check_constraints(CHECK_false);
      }
#ifdef ASSERT
      vtable().verify(tty, true);
      // In case itable verification is ever added.
      // itable().verify(tty, true);
#endif
      if (UseVtableBasedCHA) {
        MutexLocker ml(THREAD, Compile_lock);
        set_init_state(linked);

        // Now flush all code that assume the class is not linked.
        if (Universe::is_fully_initialized()) {
          CodeCache::flush_dependents_on(this);
        }
      } else {
        set_init_state(linked);
      }
      if (JvmtiExport::should_post_class_prepare()) {
        JvmtiExport::post_class_prepare(THREAD, this);
      }
    }
  }
  return true;
}