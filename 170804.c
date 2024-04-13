void InstanceKlass::set_classpath_index(s2 path_index) {
  if (_package_entry != NULL) {
    DEBUG_ONLY(PackageEntryTable* pkg_entry_tbl = ClassLoaderData::the_null_class_loader_data()->packages();)
    assert(pkg_entry_tbl->lookup_only(_package_entry->name()) == _package_entry, "Should be same");
    assert(path_index != -1, "Unexpected classpath_index");
    _package_entry->set_classpath_index(path_index);
  }
}