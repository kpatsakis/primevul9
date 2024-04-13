int InstanceKlass::find_method_by_name(const Symbol* name, int* end) const {
  return find_method_by_name(methods(), name, end);
}