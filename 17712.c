Klass* InstanceKlass::array_klass_impl(bool or_null, int n, TRAPS) {
  // Need load-acquire for lock-free read
  if (array_klasses_acquire() == NULL) {
    if (or_null) return NULL;

    ResourceMark rm;
    JavaThread *jt = (JavaThread *)THREAD;
    {
      // Atomic creation of array_klasses
      MutexLocker mc(Compile_lock, THREAD);   // for vtables
      MutexLocker ma(MultiArray_lock, THREAD);

      // Check if update has already taken place
      if (array_klasses() == NULL) {
        Klass*    k = ObjArrayKlass::allocate_objArray_klass(class_loader_data(), 1, this, CHECK_NULL);
        // use 'release' to pair with lock-free load
        release_set_array_klasses(k);
      }
    }
  }
  // _this will always be set at this point
  ObjArrayKlass* oak = (ObjArrayKlass*)array_klasses();
  if (or_null) {
    return oak->array_klass_or_null(n);
  }
  return oak->array_klass(n, THREAD);
}