objArrayOop InstanceKlass::allocate_objArray(int n, int length, TRAPS) {
  check_array_allocation_length(length, arrayOopDesc::max_array_length(T_OBJECT), CHECK_NULL);
  int size = objArrayOopDesc::object_size(length);
  Klass* ak = array_klass(n, CHECK_NULL);
  objArrayOop o = (objArrayOop)Universe::heap()->array_allocate(ak, size, length,
                                                                /* do_zero */ true, CHECK_NULL);
  return o;
}