void LinkResolver::check_field_accessability(Klass* ref_klass,
                                             Klass* resolved_klass,
                                             Klass* sel_klass,
                                             const fieldDescriptor& fd,
                                             TRAPS) {
  bool can_access = Reflection::verify_member_access(ref_klass,
                                                     resolved_klass,
                                                     sel_klass,
                                                     fd.access_flags(),
                                                     true, false, CHECK);
  // Any existing exceptions that may have been thrown, for example LinkageErrors
  // from nest-host resolution, have been allowed to propagate.
  if (!can_access) {
    bool same_module = (sel_klass->module() == ref_klass->module());
    ResourceMark rm(THREAD);
    stringStream ss;
    ss.print("class %s tried to access %s%sfield %s.%s (%s%s%s)",
             ref_klass->external_name(),
             fd.is_protected() ? "protected " : "",
             fd.is_private()   ? "private "   : "",
             sel_klass->external_name(),
             fd.name()->as_C_string(),
             (same_module) ? ref_klass->joint_in_module_of_loader(sel_klass) : ref_klass->class_in_module_of_loader(),
             (same_module) ? "" : "; ",
             (same_module) ? "" : sel_klass->class_in_module_of_loader()
             );
    // For private access see if there was a problem with nest host
    // resolution, and if so report that as part of the message.
    if (fd.is_private()) {
      print_nest_host_error_on(&ss, ref_klass, sel_klass);
    }
    Exceptions::fthrow(THREAD_AND_LOCATION,
                       vmSymbols::java_lang_IllegalAccessError(),
                       "%s",
                       ss.as_string()
                       );
    return;
  }
}