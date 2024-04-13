bool InstanceKlass::should_be_initialized() const {
  return !is_initialized();
}