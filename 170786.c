void InstanceKlass::remove_unshareable_info() {

  if (can_be_verified_at_dumptime()) {
    // Remember this so we can avoid walking the hierarchy at runtime.
    set_verified_at_dump_time();
  }

  Klass::remove_unshareable_info();

  if (SystemDictionaryShared::has_class_failed_verification(this)) {
    // Classes are attempted to link during dumping and may fail,
    // but these classes are still in the dictionary and class list in CLD.
    // If the class has failed verification, there is nothing else to remove.
    return;
  }

  // Reset to the 'allocated' state to prevent any premature accessing to
  // a shared class at runtime while the class is still being loaded and
  // restored. A class' init_state is set to 'loaded' at runtime when it's
  // being added to class hierarchy (see SystemDictionary:::add_to_hierarchy()).
  _init_state = allocated;

  { // Otherwise this needs to take out the Compile_lock.
    assert(SafepointSynchronize::is_at_safepoint(), "only called at safepoint");
    init_implementor();
  }

  constants()->remove_unshareable_info();

  for (int i = 0; i < methods()->length(); i++) {
    Method* m = methods()->at(i);
    m->remove_unshareable_info();
  }

  // do array classes also.
  if (array_klasses() != NULL) {
    array_klasses()->remove_unshareable_info();
  }

  // These are not allocated from metaspace. They are safe to set to NULL.
  _source_debug_extension = NULL;
  _dep_context = NULL;
  _osr_nmethods_head = NULL;
#if INCLUDE_JVMTI
  _breakpoints = NULL;
  _previous_versions = NULL;
  _cached_class_file = NULL;
  _jvmti_cached_class_field_map = NULL;
#endif

  _init_thread = NULL;
  _methods_jmethod_ids = NULL;
  _jni_ids = NULL;
  _oop_map_cache = NULL;
  // clear _nest_host to ensure re-load at runtime
  _nest_host = NULL;
  init_shared_package_entry();
  _dep_context_last_cleaned = 0;
}