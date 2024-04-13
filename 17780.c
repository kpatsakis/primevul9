bool InstanceKlass::check_sharing_error_state() {
  assert(DumpSharedSpaces, "should only be called during dumping");
  bool old_state = is_in_error_state();

  if (!is_in_error_state()) {
    bool bad = false;
    for (InstanceKlass* sup = java_super(); sup; sup = sup->java_super()) {
      if (sup->is_in_error_state()) {
        bad = true;
        break;
      }
    }
    if (!bad) {
      Array<Klass*>* interfaces = transitive_interfaces();
      for (int i = 0; i < interfaces->length(); i++) {
        Klass* iface = interfaces->at(i);
        if (InstanceKlass::cast(iface)->is_in_error_state()) {
          bad = true;
          break;
        }
      }
    }

    if (bad) {
      set_in_error_state();
    }
  }

  return (old_state != is_in_error_state());
}