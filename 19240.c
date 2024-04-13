void LinkResolver::resolve_field(fieldDescriptor& fd,
                                 const LinkInfo& link_info,
                                 Bytecodes::Code byte, bool initialize_class,
                                 TRAPS) {
  assert(byte == Bytecodes::_getstatic || byte == Bytecodes::_putstatic ||
         byte == Bytecodes::_getfield  || byte == Bytecodes::_putfield  ||
         byte == Bytecodes::_nofast_getfield  || byte == Bytecodes::_nofast_putfield  ||
         (byte == Bytecodes::_nop && !link_info.check_access()), "bad field access bytecode");

  bool is_static = (byte == Bytecodes::_getstatic || byte == Bytecodes::_putstatic);
  bool is_put    = (byte == Bytecodes::_putfield  || byte == Bytecodes::_putstatic || byte == Bytecodes::_nofast_putfield);
  // Check if there's a resolved klass containing the field
  Klass* resolved_klass = link_info.resolved_klass();
  Symbol* field = link_info.name();
  Symbol* sig = link_info.signature();

  if (resolved_klass == NULL) {
    ResourceMark rm(THREAD);
    THROW_MSG(vmSymbols::java_lang_NoSuchFieldError(), field->as_C_string());
  }

  // Resolve instance field
  Klass* sel_klass = resolved_klass->find_field(field, sig, &fd);
  // check if field exists; i.e., if a klass containing the field def has been selected
  if (sel_klass == NULL) {
    ResourceMark rm(THREAD);
    THROW_MSG(vmSymbols::java_lang_NoSuchFieldError(), field->as_C_string());
  }

  // Access checking may be turned off when calling from within the VM.
  Klass* current_klass = link_info.current_klass();
  if (link_info.check_access()) {

    // check access
    check_field_accessability(current_klass, resolved_klass, sel_klass, fd, CHECK);

    // check for errors
    if (is_static != fd.is_static()) {
      ResourceMark rm(THREAD);
      char msg[200];
      jio_snprintf(msg, sizeof(msg), "Expected %s field %s.%s", is_static ? "static" : "non-static", resolved_klass->external_name(), fd.name()->as_C_string());
      THROW_MSG(vmSymbols::java_lang_IncompatibleClassChangeError(), msg);
    }

    // A final field can be modified only
    // (1) by methods declared in the class declaring the field and
    // (2) by the <clinit> method (in case of a static field)
    //     or by the <init> method (in case of an instance field).
    if (is_put && fd.access_flags().is_final()) {

      if (sel_klass != current_klass) {
        ResourceMark rm(THREAD);
        stringStream ss;
        ss.print("Update to %s final field %s.%s attempted from a different class (%s) than the field's declaring class",
                 is_static ? "static" : "non-static", resolved_klass->external_name(), fd.name()->as_C_string(),
                current_klass->external_name());
        THROW_MSG(vmSymbols::java_lang_IllegalAccessError(), ss.as_string());
      }

      if (fd.constants()->pool_holder()->major_version() >= 53) {
        Method* m = link_info.current_method();
        assert(m != NULL, "information about the current method must be available for 'put' bytecodes");
        bool is_initialized_static_final_update = (byte == Bytecodes::_putstatic &&
                                                   fd.is_static() &&
                                                   !m->is_static_initializer());
        bool is_initialized_instance_final_update = ((byte == Bytecodes::_putfield || byte == Bytecodes::_nofast_putfield) &&
                                                     !fd.is_static() &&
                                                     !m->is_object_initializer());

        if (is_initialized_static_final_update || is_initialized_instance_final_update) {
          ResourceMark rm(THREAD);
          stringStream ss;
          ss.print("Update to %s final field %s.%s attempted from a different method (%s) than the initializer method %s ",
                   is_static ? "static" : "non-static", resolved_klass->external_name(), fd.name()->as_C_string(),
                   m->name()->as_C_string(),
                   is_static ? "<clinit>" : "<init>");
          THROW_MSG(vmSymbols::java_lang_IllegalAccessError(), ss.as_string());
        }
      }
    }

    // initialize resolved_klass if necessary
    // note 1: the klass which declared the field must be initialized (i.e, sel_klass)
    //         according to the newest JVM spec (5.5, p.170) - was bug (gri 7/28/99)
    //
    // note 2: we don't want to force initialization if we are just checking
    //         if the field access is legal; e.g., during compilation
    if (is_static && initialize_class) {
      sel_klass->initialize(CHECK);
    }
  }

  if (link_info.check_loader_constraints() && (sel_klass != current_klass) && (current_klass != NULL)) {
    check_field_loader_constraints(field, sig, current_klass, sel_klass, CHECK);
  }

  // return information. note that the klass is set to the actual klass containing the
  // field, otherwise access of static fields in superclasses will not work.
}