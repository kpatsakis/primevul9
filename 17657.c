JvmtiCachedClassFileData* InstanceKlass::get_archived_class_data() {
  if (DumpSharedSpaces) {
    return _cached_class_file;
  } else {
    assert(this->is_shared(), "class should be shared");
    if (MetaspaceShared::is_in_shared_metaspace(_cached_class_file)) {
      return _cached_class_file;
    } else {
      return NULL;
    }
  }
}