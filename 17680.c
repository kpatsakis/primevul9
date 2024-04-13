jmethodID InstanceKlass::get_jmethod_id(const methodHandle& method_h) {
  size_t idnum = (size_t)method_h->method_idnum();
  jmethodID* jmeths = methods_jmethod_ids_acquire();
  size_t length = 0;
  jmethodID id = NULL;

  // We use a double-check locking idiom here because this cache is
  // performance sensitive. In the normal system, this cache only
  // transitions from NULL to non-NULL which is safe because we use
  // release_set_methods_jmethod_ids() to advertise the new cache.
  // A partially constructed cache should never be seen by a racing
  // thread. We also use release_store() to save a new jmethodID
  // in the cache so a partially constructed jmethodID should never be
  // seen either. Cache reads of existing jmethodIDs proceed without a
  // lock, but cache writes of a new jmethodID requires uniqueness and
  // creation of the cache itself requires no leaks so a lock is
  // generally acquired in those two cases.
  //
  // If the RedefineClasses() API has been used, then this cache can
  // grow and we'll have transitions from non-NULL to bigger non-NULL.
  // Cache creation requires no leaks and we require safety between all
  // cache accesses and freeing of the old cache so a lock is generally
  // acquired when the RedefineClasses() API has been used.

  if (jmeths != NULL) {
    // the cache already exists
    if (!idnum_can_increment()) {
      // the cache can't grow so we can just get the current values
      get_jmethod_id_length_value(jmeths, idnum, &length, &id);
    } else {
      // cache can grow so we have to be more careful
      if (Threads::number_of_threads() == 0 ||
          SafepointSynchronize::is_at_safepoint()) {
        // we're single threaded or at a safepoint - no locking needed
        get_jmethod_id_length_value(jmeths, idnum, &length, &id);
      } else {
        MutexLockerEx ml(JmethodIdCreation_lock, Mutex::_no_safepoint_check_flag);
        get_jmethod_id_length_value(jmeths, idnum, &length, &id);
      }
    }
  }
  // implied else:
  // we need to allocate a cache so default length and id values are good

  if (jmeths == NULL ||   // no cache yet
      length <= idnum ||  // cache is too short
      id == NULL) {       // cache doesn't contain entry

    // This function can be called by the VMThread so we have to do all
    // things that might block on a safepoint before grabbing the lock.
    // Otherwise, we can deadlock with the VMThread or have a cache
    // consistency issue. These vars keep track of what we might have
    // to free after the lock is dropped.
    jmethodID  to_dealloc_id     = NULL;
    jmethodID* to_dealloc_jmeths = NULL;

    // may not allocate new_jmeths or use it if we allocate it
    jmethodID* new_jmeths = NULL;
    if (length <= idnum) {
      // allocate a new cache that might be used
      size_t size = MAX2(idnum+1, (size_t)idnum_allocated_count());
      new_jmeths = NEW_C_HEAP_ARRAY(jmethodID, size+1, mtClass);
      memset(new_jmeths, 0, (size+1)*sizeof(jmethodID));
      // cache size is stored in element[0], other elements offset by one
      new_jmeths[0] = (jmethodID)size;
    }

    // allocate a new jmethodID that might be used
    jmethodID new_id = NULL;
    if (method_h->is_old() && !method_h->is_obsolete()) {
      // The method passed in is old (but not obsolete), we need to use the current version
      Method* current_method = method_with_idnum((int)idnum);
      assert(current_method != NULL, "old and but not obsolete, so should exist");
      new_id = Method::make_jmethod_id(class_loader_data(), current_method);
    } else {
      // It is the current version of the method or an obsolete method,
      // use the version passed in
      new_id = Method::make_jmethod_id(class_loader_data(), method_h());
    }

    if (Threads::number_of_threads() == 0 ||
        SafepointSynchronize::is_at_safepoint()) {
      // we're single threaded or at a safepoint - no locking needed
      id = get_jmethod_id_fetch_or_update(idnum, new_id, new_jmeths,
                                          &to_dealloc_id, &to_dealloc_jmeths);
    } else {
      MutexLockerEx ml(JmethodIdCreation_lock, Mutex::_no_safepoint_check_flag);
      id = get_jmethod_id_fetch_or_update(idnum, new_id, new_jmeths,
                                          &to_dealloc_id, &to_dealloc_jmeths);
    }

    // The lock has been dropped so we can free resources.
    // Free up either the old cache or the new cache if we allocated one.
    if (to_dealloc_jmeths != NULL) {
      FreeHeap(to_dealloc_jmeths);
    }
    // free up the new ID since it wasn't needed
    if (to_dealloc_id != NULL) {
      Method::destroy_jmethod_id(class_loader_data(), to_dealloc_id);
    }
  }
  return id;
}