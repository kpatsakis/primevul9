void LinkResolver::check_field_loader_constraints(Symbol* field, Symbol* sig,
                                                  Klass* current_klass,
                                                  Klass* sel_klass, TRAPS) {
  Handle ref_loader(THREAD, current_klass->class_loader());
  Handle sel_loader(THREAD, sel_klass->class_loader());

  ResourceMark rm(THREAD);  // needed for check_signature_loaders
  Symbol* failed_type_symbol =
    SystemDictionary::check_signature_loaders(sig,
                                              /*klass_being_linked*/ NULL, // We are not linking class
                                              ref_loader, sel_loader,
                                              false);
  if (failed_type_symbol != NULL) {
    stringStream ss;
    const char* failed_type_name = failed_type_symbol->as_klass_external_name();

    ss.print("loader constraint violation: when resolving field \"%s\" of type %s, "
             "the class loader %s of the current class, %s, "
             "and the class loader %s for the field's defining %s, %s, "
             "have different Class objects for type %s (%s; %s)",
             field->as_C_string(),
             failed_type_name,
             current_klass->class_loader_data()->loader_name_and_id(),
             current_klass->external_name(),
             sel_klass->class_loader_data()->loader_name_and_id(),
             sel_klass->external_kind(),
             sel_klass->external_name(),
             failed_type_name,
             current_klass->class_in_module_of_loader(false, true),
             sel_klass->class_in_module_of_loader(false, true));
    THROW_MSG(vmSymbols::java_lang_LinkageError(), ss.as_string());
  }
}