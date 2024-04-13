oop InstanceKlass::init_lock() const {
  // return the init lock from the mirror
  oop lock = java_lang_Class::init_lock(java_mirror());
  // Prevent reordering with any access of initialization state
  OrderAccess::loadload();
  assert((oop)lock != NULL || !is_not_initialized(), // initialized or in_error state
         "only fully initialized state can have a null lock");
  return lock;
}