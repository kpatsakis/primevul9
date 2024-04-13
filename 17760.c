objArrayOop InstanceKlass::allocate_objArray(int n, int length, TRAPS) {
  if (length < 0)  {
    THROW_MSG_0(vmSymbols::java_lang_NegativeArraySizeException(), err_msg("%d", length));
  }
  if (length > arrayOopDesc::max_array_length(T_OBJECT)) {
    report_java_out_of_memory("Requested array size exceeds VM limit");
    JvmtiExport::post_array_size_exhausted();
    THROW_OOP_0(Universe::out_of_memory_error_array_size());
  }
  int size = objArrayOopDesc::object_size(length);
  Klass* ak = array_klass(n, CHECK_NULL);
  objArrayOop o = (objArrayOop)Universe::heap()->array_allocate(ak, size, length,
                                                                /* do_zero */ true, CHECK_NULL);
  return o;
}