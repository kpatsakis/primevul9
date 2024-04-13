Klass* InstanceKlass::array_klass(int n, TRAPS) {
  // Need load-acquire for lock-free read
  if (array_klasses_acquire() == NULL) {
    ResourceMark rm(THREAD);
    JavaThread *jt = THREAD;
    {
      // Atomic creation of array_klasses
      MutexLocker ma(THREAD, MultiArray_lock);

      // Check if update has already taken place
      if (array_klasses() == NULL) {
        ObjArrayKlass* k = ObjArrayKlass::allocate_objArray_klass(class_loader_data(), 1, this, CHECK_NULL);
        // use 'release' to pair with lock-free load
        release_set_array_klasses(k);
      }
    }
  }
  // array_klasses() will always be set at this point
  ObjArrayKlass* oak = array_klasses();
  return oak->array_klass(n, THREAD);
}