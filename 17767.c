void InstanceKlass::init_implementor() {
  if (is_interface()) {
    set_implementor(NULL);
  }
}