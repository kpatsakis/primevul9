void InstanceKlass::fence_and_clear_init_lock() {
  // make sure previous stores are all done, notably the init_state.
  OrderAccess::storestore();
  java_lang_Class::clear_init_lock(java_mirror());
  assert(!is_not_initialized(), "class must be initialized now");
}