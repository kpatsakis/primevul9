void InstanceKlass::restore_unshareable_info(ClassLoaderData* loader_data, Handle protection_domain, TRAPS) {
  // SystemDictionary::add_to_hierarchy() sets the init_state to loaded
  // before the InstanceKlass is added to the SystemDictionary. Make
  // sure the current state is <loaded.
  assert(!is_loaded(), "invalid init state");
  set_package(loader_data, CHECK);
  Klass::restore_unshareable_info(loader_data, protection_domain, CHECK);

  Array<Method*>* methods = this->methods();
  int num_methods = methods->length();
  for (int index2 = 0; index2 < num_methods; ++index2) {
    methodHandle m(THREAD, methods->at(index2));
    m->restore_unshareable_info(CHECK);
  }
  if (JvmtiExport::has_redefined_a_class()) {
    // Reinitialize vtable because RedefineClasses may have changed some
    // entries in this vtable for super classes so the CDS vtable might
    // point to old or obsolete entries.  RedefineClasses doesn't fix up
    // vtables in the shared system dictionary, only the main one.
    // It also redefines the itable too so fix that too.
    ResourceMark rm(THREAD);
    vtable().initialize_vtable(false, CHECK);
    itable().initialize_itable(false, CHECK);
  }

  // restore constant pool resolved references
  constants()->restore_unshareable_info(CHECK);

  if (array_klasses() != NULL) {
    // Array classes have null protection domain.
    // --> see ArrayKlass::complete_create_array_klass()
    array_klasses()->restore_unshareable_info(ClassLoaderData::the_null_class_loader_data(), Handle(), CHECK);
  }
}