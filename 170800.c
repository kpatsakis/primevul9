bool InstanceKlass::contains_field_offset(int offset) {
  fieldDescriptor fd;
  return find_field_from_offset(offset, false, &fd);
}