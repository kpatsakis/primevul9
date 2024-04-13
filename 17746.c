int InstanceKlass::find_method_by_name(const Array<Method*>* methods,
                                       const Symbol* name,
                                       int* end_ptr) {
  assert(end_ptr != NULL, "just checking");
  int start = binary_search(methods, name);
  int end = start + 1;
  if (start != -1) {
    while (start - 1 >= 0 && (methods->at(start - 1))->name() == name) --start;
    while (end < methods->length() && (methods->at(end))->name() == name) ++end;
    *end_ptr = end;
    return start;
  }
  return -1;
}