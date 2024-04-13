u2 InstanceKlass::enclosing_method_data(int offset) const {
  const Array<jushort>* const inner_class_list = inner_classes();
  if (inner_class_list == NULL) {
    return 0;
  }
  const int length = inner_class_list->length();
  if (length % inner_class_next_offset == 0) {
    return 0;
  }
  const int index = length - enclosing_method_attribute_size;
  assert(offset < enclosing_method_attribute_size, "invalid offset");
  return inner_class_list->at(index + offset);
}