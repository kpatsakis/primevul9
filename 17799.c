bool InstanceKlass::has_stored_fingerprint() const {
#if INCLUDE_AOT
  return should_store_fingerprint() || is_shared();
#else
  return false;
#endif
}