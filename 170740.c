InstanceKlass* InstanceKlass::implementor() const {
  InstanceKlass* volatile* ik = adr_implementor();
  if (ik == NULL) {
    return NULL;
  } else {
    // This load races with inserts, and therefore needs acquire.
    InstanceKlass* ikls = Atomic::load_acquire(ik);
    if (ikls != NULL && !ikls->is_loader_alive()) {
      return NULL;  // don't return unloaded class
    } else {
      return ikls;
    }
  }
}