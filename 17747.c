bool InstanceKlass::find_field_from_offset(int offset, bool is_static, fieldDescriptor* fd) const {
  Klass* klass = const_cast<InstanceKlass*>(this);
  while (klass != NULL) {
    if (InstanceKlass::cast(klass)->find_local_field_from_offset(offset, is_static, fd)) {
      return true;
    }
    klass = klass->super();
  }
  return false;
}