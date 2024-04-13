void LinkResolver::check_method_accessability(Klass* ref_klass,
                                              Klass* resolved_klass,
                                              Klass* sel_klass,
                                              const methodHandle& sel_method,
                                              TRAPS) {

  AccessFlags flags = sel_method->access_flags();

  // Special case:  arrays always override "clone". JVMS 2.15.
  // If the resolved klass is an array class, and the declaring class
  // is java.lang.Object and the method is "clone", set the flags
  // to public.
  //
  // We'll check for the method name first, as that's most likely
  // to be false (so we'll short-circuit out of these tests).
  if (sel_method->name() == vmSymbols::clone_name() &&
      sel_klass == vmClasses::Object_klass() &&
      resolved_klass->is_array_klass()) {
    // We need to change "protected" to "public".
    assert(flags.is_protected(), "clone not protected?");
    jint new_flags = flags.as_int();
    new_flags = new_flags & (~JVM_ACC_PROTECTED);
    new_flags = new_flags | JVM_ACC_PUBLIC;
    flags.set_flags(new_flags);
  }
//  assert(extra_arg_result_or_null != NULL, "must be able to return extra argument");

  bool can_access = Reflection::verify_member_access(ref_klass,
                                                     resolved_klass,
                                                     sel_klass,
                                                     flags,
                                                     true, false, CHECK);
  // Any existing exceptions that may have been thrown
  // have been allowed to propagate.
  if (!can_access) {
    ResourceMark rm(THREAD);
    stringStream ss;
    bool same_module = (sel_klass->module() == ref_klass->module());
    ss.print("class %s tried to access %s%s%smethod '%s' (%s%s%s)",
             ref_klass->external_name(),
             sel_method->is_abstract()  ? "abstract "  : "",
             sel_method->is_protected() ? "protected " : "",
             sel_method->is_private()   ? "private "   : "",
             sel_method->external_name(),
             (same_module) ? ref_klass->joint_in_module_of_loader(sel_klass) : ref_klass->class_in_module_of_loader(),
             (same_module) ? "" : "; ",
             (same_module) ? "" : sel_klass->class_in_module_of_loader()
             );

    // For private access see if there was a problem with nest host
    // resolution, and if so report that as part of the message.
    if (sel_method->is_private()) {
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