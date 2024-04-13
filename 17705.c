void InstanceKlass::set_initialization_state_and_notify(ClassState state, TRAPS) {
  Handle h_init_lock(THREAD, init_lock());
  if (h_init_lock() != NULL) {
    ObjectLocker ol(h_init_lock, THREAD);
    set_init_thread(NULL); // reset _init_thread before changing _init_state
    set_init_state(state);
    fence_and_clear_init_lock();
    ol.notify_all(CHECK);
  } else {
    assert(h_init_lock() != NULL, "The initialization state should never be set twice");
    set_init_thread(NULL); // reset _init_thread before changing _init_state
    set_init_state(state);
  }
}