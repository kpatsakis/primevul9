void InstanceKlass::initialize(TRAPS) {
  if (this->should_be_initialized()) {
    initialize_impl(CHECK);
    // Note: at this point the class may be initialized
    //       OR it may be in the state of being initialized
    //       in case of recursive initialization!
  } else {
    assert(is_initialized(), "sanity check");
  }
}