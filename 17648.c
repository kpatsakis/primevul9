Method* InstanceKlass::method_with_orig_idnum(int idnum, int version) {
  InstanceKlass* holder = get_klass_version(version);
  if (holder == NULL) {
    return NULL; // The version of klass is gone, no method is found
  }
  Method* method = holder->method_with_orig_idnum(idnum);
  return method;
}