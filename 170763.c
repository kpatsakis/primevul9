void InstanceKlass::release_C_heap_structures_internal() {
  Klass::release_C_heap_structures();

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

  assert(_dep_context == NULL,
         "dependencies should already be cleaned");

#if INCLUDE_JVMTI
  // Deallocate breakpoint records
  if (breakpoints() != 0x0) {
    methods_do(clear_all_breakpoints);
    assert(breakpoints() == 0x0, "should have cleared breakpoints");
  }

  // deallocate the cached class file
  if (_cached_class_file != NULL) {
    os::free(_cached_class_file);
    _cached_class_file = NULL;
  }
#endif

  FREE_C_HEAP_ARRAY(char, _source_debug_extension);
}