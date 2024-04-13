void InstanceKlass::do_local_static_fields(void f(fieldDescriptor*, Handle, TRAPS), Handle mirror, TRAPS) {
  for (JavaFieldStream fs(this); !fs.done(); fs.next()) {
    if (fs.access_flags().is_static()) {
      fieldDescriptor& fd = fs.field_descriptor();
      f(&fd, mirror, CHECK);
    }
  }
}