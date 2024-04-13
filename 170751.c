inline int InstanceKlass::quick_search(const Array<Method*>* methods, const Symbol* name) {
  if (_disable_method_binary_search) {
    assert(DynamicDumpSharedSpaces, "must be");
    // At the final stage of dynamic dumping, the methods array may not be sorted
    // by ascending addresses of their names, so we can't use binary search anymore.
    // However, methods with the same name are still laid out consecutively inside the
    // methods array, so let's look for the first one that matches.
    return linear_search(methods, name);
  }

  int len = methods->length();
  int l = 0;
  int h = len - 1;

  // methods are sorted by ascending addresses of their names, so do binary search
  while (l <= h) {
    int mid = (l + h) >> 1;
    Method* m = methods->at(mid);
    assert(m->is_method(), "must be method");
    int res = m->name()->fast_compare(name);
    if (res == 0) {
      return mid;
    } else if (res < 0) {
      l = mid + 1;
    } else {
      h = mid - 1;
    }
  }
  return -1;
}