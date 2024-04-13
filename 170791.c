void InstanceKlass::eager_initialize_impl() {
  EXCEPTION_MARK;
  HandleMark hm(THREAD);
  Handle h_init_lock(THREAD, init_lock());
  ObjectLocker ol(h_init_lock, THREAD);

  // abort if someone beat us to the initialization
  if (!is_not_initialized()) return;  // note: not equivalent to is_initialized()

  ClassState old_state = init_state();
  link_class_impl(THREAD);
  if (HAS_PENDING_EXCEPTION) {
    CLEAR_PENDING_EXCEPTION;
    // Abort if linking the class throws an exception.

    // Use a test to avoid redundantly resetting the state if there's
    // no change.  Set_init_state() asserts that state changes make
    // progress, whereas here we might just be spinning in place.
    if (old_state != _init_state)
      set_init_state(old_state);
  } else {
    // linking successfull, mark class as initialized
    set_init_state(fully_initialized);
    fence_and_clear_init_lock();
    // trace
    if (log_is_enabled(Info, class, init)) {
      ResourceMark rm(THREAD);
      log_info(class, init)("[Initialized %s without side effects]", external_name());
    }
  }
}