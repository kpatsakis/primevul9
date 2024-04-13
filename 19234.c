void LinkResolver::check_method_loader_constraints(const LinkInfo& link_info,
                                                   const methodHandle& resolved_method,
                                                   const char* method_type, TRAPS) {
  Handle current_loader(THREAD, link_info.current_klass()->class_loader());
  Handle resolved_loader(THREAD, resolved_method->method_holder()->class_loader());

  ResourceMark rm(THREAD);
  Symbol* failed_type_symbol =
    SystemDictionary::check_signature_loaders(link_info.signature(),
                                              /*klass_being_linked*/ NULL, // We are not linking class
                                              current_loader,
                                              resolved_loader, true);
  if (failed_type_symbol != NULL) {
    Klass* current_class = link_info.current_klass();
    ClassLoaderData* current_loader_data = current_class->class_loader_data();
    assert(current_loader_data != NULL, "current class has no class loader data");
    Klass* resolved_method_class = resolved_method->method_holder();
    ClassLoaderData* target_loader_data = resolved_method_class->class_loader_data();
    assert(target_loader_data != NULL, "resolved method's class has no class loader data");

    stringStream ss;
    ss.print("loader constraint violation: when resolving %s '", method_type);
    Method::print_external_name(&ss, link_info.resolved_klass(), link_info.name(), link_info.signature());
    ss.print("' the class loader %s of the current class, %s,"
             " and the class loader %s for the method's defining class, %s, have"
             " different Class objects for the type %s used in the signature (%s; %s)",
             current_loader_data->loader_name_and_id(),
             current_class->name()->as_C_string(),
             target_loader_data->loader_name_and_id(),
             resolved_method_class->name()->as_C_string(),
             failed_type_symbol->as_C_string(),
             current_class->class_in_module_of_loader(false, true),
             resolved_method_class->class_in_module_of_loader(false, true));
    THROW_MSG(vmSymbols::java_lang_LinkageError(), ss.as_string());
  }
}