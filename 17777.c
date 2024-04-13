void InstanceKlass::methods_do(void f(Method* method)) {
  // Methods aren't stable until they are loaded.  This can be read outside
  // a lock through the ClassLoaderData for profiling
  if (!is_loaded()) {
    return;
  }

  int len = methods()->length();
  for (int index = 0; index < len; index++) {
    Method* m = methods()->at(index);
    assert(m->is_method(), "must be method");
    f(m);
  }
}