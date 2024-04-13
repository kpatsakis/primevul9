void InstanceKlass::link_methods(TRAPS) {
  int len = methods()->length();
  for (int i = len-1; i >= 0; i--) {
    methodHandle m(THREAD, methods()->at(i));

    // Set up method entry points for compiler and interpreter    .
    m->link_method(m, CHECK);
  }
}