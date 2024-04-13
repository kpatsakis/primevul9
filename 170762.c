NOINLINE int linear_search(const Array<Method*>* methods, const Symbol* name) {
  int len = methods->length();
  int l = 0;
  int h = len - 1;
  while (l <= h) {
    Method* m = methods->at(l);
    if (m->name() == name) {
      return l;
    }
    l++;
  }
  return -1;
}