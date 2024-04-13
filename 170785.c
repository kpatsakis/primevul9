void InstanceKlass::init_shared_package_entry() {
#if !INCLUDE_CDS_JAVA_HEAP
  _package_entry = NULL;
#else
  if (!MetaspaceShared::use_full_module_graph()) {
    _package_entry = NULL;
  } else if (DynamicDumpSharedSpaces) {
    if (!MetaspaceShared::is_in_shared_metaspace(_package_entry)) {
      _package_entry = NULL;
    }
  } else {
    if (is_shared_unregistered_class()) {
      _package_entry = NULL;
    } else {
      _package_entry = PackageEntry::get_archived_entry(_package_entry);
    }
  }
  ArchivePtrMarker::mark_pointer((address**)&_package_entry);
#endif
}