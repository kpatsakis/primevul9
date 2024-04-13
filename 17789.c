void JNIid::verify(Klass* holder) {
  int first_field_offset  = InstanceMirrorKlass::offset_of_static_fields();
  int end_field_offset;
  end_field_offset = first_field_offset + (InstanceKlass::cast(holder)->static_field_size() * wordSize);

  JNIid* current = this;
  while (current != NULL) {
    guarantee(current->holder() == holder, "Invalid klass in JNIid");
#ifdef ASSERT
    int o = current->offset();
    if (current->is_static_field_id()) {
      guarantee(o >= first_field_offset  && o < end_field_offset,  "Invalid static field offset in JNIid");
    }
#endif
    current = current->next();
  }
}