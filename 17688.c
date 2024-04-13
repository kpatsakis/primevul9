Klass* InstanceKlass::find_interface_field(Symbol* name, Symbol* sig, fieldDescriptor* fd) const {
  const int n = local_interfaces()->length();
  for (int i = 0; i < n; i++) {
    Klass* intf1 = local_interfaces()->at(i);
    assert(intf1->is_interface(), "just checking type");
    // search for field in current interface
    if (InstanceKlass::cast(intf1)->find_local_field(name, sig, fd)) {
      assert(fd->is_static(), "interface field must be static");
      return intf1;
    }
    // search for field in direct superinterfaces
    Klass* intf2 = InstanceKlass::cast(intf1)->find_interface_field(name, sig, fd);
    if (intf2 != NULL) return intf2;
  }
  // otherwise field lookup fails
  return NULL;
}