static int linear_search(const Array<Method*>* methods,
                         const Symbol* name,
                         const Symbol* signature) {
  const int len = methods->length();
  for (int index = 0; index < len; index++) {
    const Method* const m = methods->at(index);
    assert(m->is_method(), "must be method");
    if (m->signature() == signature && m->name() == name) {
       return index;
    }
  }
  return -1;
}