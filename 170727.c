Klass* InstanceKlass::array_klass_or_null(int n) {
  // Need load-acquire for lock-free read
  ObjArrayKlass* oak = array_klasses_acquire();
  if (oak == NULL) {
    return NULL;
  } else {
    return oak->array_klass_or_null(n);
  }
}