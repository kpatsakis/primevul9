void InstanceKlass::array_klasses_do(void f(Klass* k, TRAPS), TRAPS) {
  if (array_klasses() != NULL)
    array_klasses()->array_klasses_do(f, THREAD);
}