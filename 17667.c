ModuleEntry* InstanceKlass::module() const {
  if (!in_unnamed_package()) {
    return _package_entry->module();
  }
  const Klass* host = host_klass();
  if (host == NULL) {
    return class_loader_data()->unnamed_module();
  }
  return host->class_loader_data()->unnamed_module();
}