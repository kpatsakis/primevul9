void InstanceKlass::set_init_state(ClassState state) {
#ifdef ASSERT
  bool good_state = is_shared() ? (_init_state <= state)
                                               : (_init_state < state);
  assert(good_state || state == allocated, "illegal state transition");
#endif
  assert(_init_thread == NULL, "should be cleared before state change");
  _init_state = (u1)state;
}