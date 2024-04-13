void InstanceKlass::remove_unshareable_info() {
  Klass::remove_unshareable_info();

  if (is_in_error_state()) {
    // Classes are attempted to link during dumping and may fail,
    // but these classes are still in the dictionary and class list in CLD.
    // Check in_error state first because in_error is > linked state, so
    // is_linked() is true.
    // If there's a linking error, there is nothing else to remove.
    return;
  }

  // Reset to the 'allocated' state to prevent any premature accessing to
  // a shared class at runtime while the class is still being loaded and
  // restored. A class' init_state is set to 'loaded' at runtime when it's
  // being added to class hierarchy (see SystemDictionary:::add_to_hierarchy()).
  _init_state = allocated;

  {
    MutexLocker ml(Compile_lock);
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

  // These are not allocated from metaspace, but they should should all be empty
  // during dump time, so we don't need to worry about them in InstanceKlass::iterate().
  guarantee(_source_debug_extension == NULL, "must be");
  guarantee(_dep_context == DependencyContext::EMPTY, "must be");
  guarantee(_osr_nmethods_head == NULL, "must be");

#if INCLUDE_JVMTI
  guarantee(_breakpoints == NULL, "must be");
  guarantee(_previous_versions == NULL, "must be");
#endif

  _init_thread = NULL;
  _methods_jmethod_ids = NULL;
  _jni_ids = NULL;
  _oop_map_cache = NULL;
  // clear _nest_host to ensure re-load at runtime
  _nest_host = NULL;
}