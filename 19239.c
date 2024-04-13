int LinkResolver::vtable_index_of_interface_method(Klass* klass, const methodHandle& resolved_method) {
  InstanceKlass* ik = InstanceKlass::cast(klass);
  return ik->vtable_index_of_interface_method(resolved_method());
}