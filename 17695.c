void InstanceKlass::release_C_heap_structures() {
  // Can't release the constant pool here because the constant pool can be
  // deallocated separately from the InstanceKlass for default methods and
  // redefine classes.

  // Deallocate oop map cache
  if (_oop_map_cache != NULL) {
    delete _oop_map_cache;
    _oop_map_cache = NULL;
  }

  // Deallocate JNI identifiers for jfieldIDs
  JNIid::deallocate(jni_ids());
  set_jni_ids(NULL);

  jmethodID* jmeths = methods_jmethod_ids_acquire();
  if (jmeths != (jmethodID*)NULL) {
    release_set_methods_jmethod_ids(NULL);
    FreeHeap(jmeths);
  }

  // Release dependencies.
  // It is desirable to use DC::remove_all_dependents() here, but, unfortunately,
  // it is not safe (see JDK-8143408). The problem is that the klass dependency
  // context can contain live dependencies, since there's a race between nmethod &
  // klass unloading. If the klass is dead when nmethod unloading happens, relevant
  // dependencies aren't removed from the context associated with the class (see
  // nmethod::flush_dependencies). It ends up during klass unloading as seemingly
  // live dependencies pointing to unloaded nmethods and causes a crash in
  // DC::remove_all_dependents() when it touches unloaded nmethod.
  dependencies().wipe();

#if INCLUDE_JVMTI
  // Deallocate breakpoint records
  if (breakpoints() != 0x0) {
    methods_do(clear_all_breakpoints);
    assert(breakpoints() == 0x0, "should have cleared breakpoints");
  }

  // deallocate the cached class file
  if (_cached_class_file != NULL && !MetaspaceShared::is_in_shared_metaspace(_cached_class_file)) {
    os::free(_cached_class_file);
    _cached_class_file = NULL;
  }
#endif

  // Decrement symbol reference counts associated with the unloaded class.
  if (_name != NULL) _name->decrement_refcount();
  // unreference array name derived from this class name (arrays of an unloaded
  // class can't be referenced anymore).
  if (_array_name != NULL)  _array_name->decrement_refcount();
  if (_source_debug_extension != NULL) FREE_C_HEAP_ARRAY(char, _source_debug_extension);
}