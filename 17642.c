Method* InstanceKlass::method_with_idnum(int idnum) {
  Method* m = NULL;
  if (idnum < methods()->length()) {
    m = methods()->at(idnum);
  }
  if (m == NULL || m->method_idnum() != idnum) {
    for (int index = 0; index < methods()->length(); ++index) {
      m = methods()->at(index);
      if (m->method_idnum() == idnum) {
        return m;
      }
    }
    // None found, return null for the caller to handle.
    return NULL;
  }
  return m;
}