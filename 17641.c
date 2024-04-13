void InstanceKlass::remove_java_mirror() {
  Klass::remove_java_mirror();

  // do array classes also.
  if (array_klasses() != NULL) {
    array_klasses()->remove_java_mirror();
  }
}