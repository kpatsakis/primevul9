instanceHandle InstanceKlass::allocate_instance_handle(TRAPS) {
  return instanceHandle(THREAD, allocate_instance(THREAD));
}