jmethodID InstanceKlass::get_jmethod_id_fetch_or_update(
            size_t idnum, jmethodID new_id,
            jmethodID* new_jmeths, jmethodID* to_dealloc_id_p,
            jmethodID** to_dealloc_jmeths_p) {
  assert(new_id != NULL, "sanity check");
  assert(to_dealloc_id_p != NULL, "sanity check");
  assert(to_dealloc_jmeths_p != NULL, "sanity check");
  assert(Threads::number_of_threads() == 0 ||
         SafepointSynchronize::is_at_safepoint() ||
         JmethodIdCreation_lock->owned_by_self(), "sanity check");

  // reacquire the cache - we are locked, single threaded or at a safepoint
  jmethodID* jmeths = methods_jmethod_ids_acquire();
  jmethodID  id     = NULL;
  size_t     length = 0;

  if (jmeths == NULL ||                         // no cache yet
      (length = (size_t)jmeths[0]) <= idnum) {  // cache is too short
    if (jmeths != NULL) {
      // copy any existing entries from the old cache
      for (size_t index = 0; index < length; index++) {
        new_jmeths[index+1] = jmeths[index+1];
      }
      *to_dealloc_jmeths_p = jmeths;  // save old cache for later delete
    }
    release_set_methods_jmethod_ids(jmeths = new_jmeths);
  } else {
    // fetch jmethodID (if any) from the existing cache
    id = jmeths[idnum+1];
    *to_dealloc_jmeths_p = new_jmeths;  // save new cache for later delete
  }
  if (id == NULL) {
    // No matching jmethodID in the existing cache or we have a new
    // cache or we just grew the cache. This cache write is done here
    // by the first thread to win the foot race because a jmethodID
    // needs to be unique once it is generally available.
    id = new_id;

    // The jmethodID cache can be read while unlocked so we have to
    // make sure the new jmethodID is complete before installing it
    // in the cache.
    OrderAccess::release_store(&jmeths[idnum+1], id);
  } else {
    *to_dealloc_id_p = new_id; // save new id for later delete
  }
  return id;
}