Method* InstanceKlass::method_with_orig_idnum(int idnum) {
  if (idnum >= methods()->length()) {
    return NULL;
  }
  Method* m = methods()->at(idnum);
  if (m != NULL && m->orig_method_idnum() == idnum) {
    return m;
  }
  // Obsolete method idnum does not match the original idnum
  for (int index = 0; index < methods()->length(); ++index) {
    m = methods()->at(index);
    if (m->orig_method_idnum() == idnum) {
      return m;
    }
  }
  // None found, return null for the caller to handle.
  return NULL;
}