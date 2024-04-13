JvmtiCachedClassFileData* InstanceKlass::get_cached_class_file() {
  if (MetaspaceShared::is_in_shared_metaspace(_cached_class_file)) {
    // Ignore the archived class stream data
    return NULL;
  } else {
    return _cached_class_file;
  }
}