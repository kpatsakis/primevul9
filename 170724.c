ModuleEntry* InstanceKlass::module() const {
  if (is_hidden() &&
      in_unnamed_package() &&
      class_loader_data()->has_class_mirror_holder()) {
    // For a non-strong hidden class defined to an unnamed package,
    // its (class held) CLD will not have an unnamed module created for it.
    // Two choices to find the correct ModuleEntry:
    // 1. If hidden class is within a nest, use nest host's module
    // 2. Find the unnamed module off from the class loader
    // For now option #2 is used since a nest host is not set until
    // after the instance class is created in jvm_lookup_define_class().
    if (class_loader_data()->is_boot_class_loader_data()) {
      return ClassLoaderData::the_null_class_loader_data()->unnamed_module();
    } else {
      oop module = java_lang_ClassLoader::unnamedModule(class_loader_data()->class_loader());
      assert(java_lang_Module::is_instance(module), "Not an instance of java.lang.Module");
      return java_lang_Module::module_entry(module);
    }
  }

  // Class is in a named package
  if (!in_unnamed_package()) {
    return _package_entry->module();
  }

  // Class is in an unnamed package, return its loader's unnamed module
  return class_loader_data()->unnamed_module();
}