void InstanceKlass::array_klasses_do(void f(Klass* k)) {
  if (array_klasses() != NULL)
    ArrayKlass::cast(array_klasses())->array_klasses_do(f);
}