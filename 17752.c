unsigned char * InstanceKlass::get_cached_class_file_bytes() {
  return VM_RedefineClasses::get_cached_class_file_bytes(_cached_class_file);
}