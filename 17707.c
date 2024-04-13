Klass* InstanceKlass::find_field(Symbol* name, Symbol* sig, fieldDescriptor* fd) const {
  // search order according to newest JVM spec (5.4.3.2, p.167).
  // 1) search for field in current klass
  if (find_local_field(name, sig, fd)) {
    return const_cast<InstanceKlass*>(this);
  }
  // 2) search for field recursively in direct superinterfaces
  { Klass* intf = find_interface_field(name, sig, fd);
    if (intf != NULL) return intf;
  }
  // 3) apply field lookup recursively if superclass exists
  { Klass* supr = super();
    if (supr != NULL) return InstanceKlass::cast(supr)->find_field(name, sig, fd);
  }
  // 4) otherwise field lookup fails
  return NULL;
}