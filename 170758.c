void InstanceKlass::restore_unshareable_info(ClassLoaderData* loader_data, Handle protection_domain,
                                             PackageEntry* pkg_entry, TRAPS) {
  // SystemDictionary::add_to_hierarchy() sets the init_state to loaded
  // before the InstanceKlass is added to the SystemDictionary. Make
  // sure the current state is <loaded.
  assert(!is_loaded(), "invalid init state");
  assert(!shared_loading_failed(), "Must not try to load failed class again");
  set_package(loader_data, pkg_entry, CHECK);
  Klass::restore_unshareable_info(loader_data, protection_domain, CHECK);

  Array<Method*>* methods = this->methods();
  int num_methods = methods->length();
  for (int index = 0; index < num_methods; ++index) {
    methods->at(index)->restore_unshareable_info(CHECK);
  }
#if INCLUDE_JVMTI
  if (JvmtiExport::has_redefined_a_class()) {
    // Reinitialize vtable because RedefineClasses may have changed some
    // entries in this vtable for super classes so the CDS vtable might
    // point to old or obsolete entries.  RedefineClasses doesn't fix up
    // vtables in the shared system dictionary, only the main one.
    // It also redefines the itable too so fix that too.
    // First fix any default methods that point to a super class that may
    // have been redefined.
    bool trace_name_printed = false;
    adjust_default_methods(&trace_name_printed);
    vtable().initialize_vtable();
    itable().initialize_itable();
  }
#endif

  // restore constant pool resolved references
  constants()->restore_unshareable_info(CHECK);

  if (array_klasses() != NULL) {
    // Array classes have null protection domain.
    // --> see ArrayKlass::complete_create_array_klass()
    array_klasses()->restore_unshareable_info(ClassLoaderData::the_null_class_loader_data(), Handle(), CHECK);
  }

  // Initialize current biased locking state.
  if (UseBiasedLocking && BiasedLocking::enabled()) {
    set_prototype_header(markWord::biased_locking_prototype());
  }

  // Initialize @ValueBased class annotation
  if (DiagnoseSyncOnValueBasedClasses && has_value_based_class_annotation()) {
    set_is_value_based();
    set_prototype_header(markWord::prototype());
  }
}