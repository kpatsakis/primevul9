void InstanceKlass::clean_implementors_list() {
  assert(is_loader_alive(), "this klass should be live");
  if (is_interface()) {
    if (ClassUnloading) {
      Klass* impl = implementor();
      if (impl != NULL) {
        if (!impl->is_loader_alive()) {
          // remove this guy
          Klass** klass = adr_implementor();
          assert(klass != NULL, "null klass");
          if (klass != NULL) {
            *klass = NULL;
          }
        }
      }
    }
  }
}