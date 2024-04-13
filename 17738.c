Klass* InstanceKlass::array_klass_impl(bool or_null, TRAPS) {
  return array_klass_impl(or_null, 1, THREAD);
}