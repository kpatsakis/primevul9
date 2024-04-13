void InstanceKlass::mask_for(const methodHandle& method, int bci,
  InterpreterOopMap* entry_for) {
  // Lazily create the _oop_map_cache at first request
  // Lock-free access requires load_acquire.
  OopMapCache* oop_map_cache = OrderAccess::load_acquire(&_oop_map_cache);
  if (oop_map_cache == NULL) {
    MutexLocker x(OopMapCacheAlloc_lock);
    // Check if _oop_map_cache was allocated while we were waiting for this lock
    if ((oop_map_cache = _oop_map_cache) == NULL) {
      oop_map_cache = new OopMapCache();
      // Ensure _oop_map_cache is stable, since it is examined without a lock
      OrderAccess::release_store(&_oop_map_cache, oop_map_cache);
    }
  }
  // _oop_map_cache is constant after init; lookup below does its own locking.
  oop_map_cache->lookup(method, bci, entry_for);
}