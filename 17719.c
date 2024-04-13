void InstanceKlass::set_package(ClassLoaderData* loader_data, TRAPS) {

  // ensure java/ packages only loaded by boot or platform builtin loaders
  check_prohibited_package(name(), loader_data, CHECK);

  TempNewSymbol pkg_name = package_from_name(name(), CHECK);

  if (pkg_name != NULL && loader_data != NULL) {

    // Find in class loader's package entry table.
    _package_entry = loader_data->packages()->lookup_only(pkg_name);

    // If the package name is not found in the loader's package
    // entry table, it is an indication that the package has not
    // been defined. Consider it defined within the unnamed module.
    if (_package_entry == NULL) {
      ResourceMark rm;

      if (!ModuleEntryTable::javabase_defined()) {
        // Before java.base is defined during bootstrapping, define all packages in
        // the java.base module.  If a non-java.base package is erroneously placed
        // in the java.base module it will be caught later when java.base
        // is defined by ModuleEntryTable::verify_javabase_packages check.
        assert(ModuleEntryTable::javabase_moduleEntry() != NULL, JAVA_BASE_NAME " module is NULL");
        _package_entry = loader_data->packages()->lookup(pkg_name, ModuleEntryTable::javabase_moduleEntry());
      } else {
        assert(loader_data->unnamed_module() != NULL, "unnamed module is NULL");
        _package_entry = loader_data->packages()->lookup(pkg_name,
                                                         loader_data->unnamed_module());
      }

      // A package should have been successfully created
      assert(_package_entry != NULL, "Package entry for class %s not found, loader %s",
             name()->as_C_string(), loader_data->loader_name_and_id());
    }

    if (log_is_enabled(Debug, module)) {
      ResourceMark rm;
      ModuleEntry* m = _package_entry->module();
      log_trace(module)("Setting package: class: %s, package: %s, loader: %s, module: %s",
                        external_name(),
                        pkg_name->as_C_string(),
                        loader_data->loader_name_and_id(),
                        (m->is_named() ? m->name()->as_C_string() : UNNAMED_MODULE));
    }
  } else {
    ResourceMark rm;
    log_trace(module)("Setting package: class: %s, package: unnamed, loader: %s, module: %s",
                      external_name(),
                      (loader_data != NULL) ? loader_data->loader_name_and_id() : "NULL",
                      UNNAMED_MODULE);
  }
}