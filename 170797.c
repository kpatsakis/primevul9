void InstanceKlass::clean_implementors_list() {
  assert(is_loader_alive(), "this klass should be live");
  if (is_interface()) {
    assert (ClassUnloading, "only called for ClassUnloading");
    for (;;) {
      // Use load_acquire due to competing with inserts
      InstanceKlass* impl = Atomic::load_acquire(adr_implementor());
      if (impl != NULL && !impl->is_loader_alive()) {
        // NULL this field, might be an unloaded instance klass or NULL
        InstanceKlass* volatile* iklass = adr_implementor();
        if (Atomic::cmpxchg(iklass, impl, (InstanceKlass*)NULL) == impl) {
          // Successfully unlinking implementor.
          if (log_is_enabled(Trace, class, unload)) {
            ResourceMark rm;
            log_trace(class, unload)("unlinking class (implementor): %s", impl->external_name());
          }
          return;
        }
      } else {
        return;
      }
    }
  }
}