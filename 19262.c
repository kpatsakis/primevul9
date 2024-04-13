void LinkResolver::check_klass_accessibility(Klass* ref_klass, Klass* sel_klass, TRAPS) {
  Klass* base_klass = sel_klass;
  if (sel_klass->is_objArray_klass()) {
    base_klass = ObjArrayKlass::cast(sel_klass)->bottom_klass();
  }
  // The element type could be a typeArray - we only need the access
  // check if it is a reference to another class.
  if (!base_klass->is_instance_klass()) {
    return;  // no relevant check to do
  }

  Reflection::VerifyClassAccessResults vca_result =
    Reflection::verify_class_access(ref_klass, InstanceKlass::cast(base_klass), true);
  if (vca_result != Reflection::ACCESS_OK) {
    ResourceMark rm(THREAD);
    char* msg = Reflection::verify_class_access_msg(ref_klass,
                                                    InstanceKlass::cast(base_klass),
                                                    vca_result);
    bool same_module = (base_klass->module() == ref_klass->module());
    if (msg == NULL) {
      Exceptions::fthrow(
        THREAD_AND_LOCATION,
        vmSymbols::java_lang_IllegalAccessError(),
        "failed to access class %s from class %s (%s%s%s)",
        base_klass->external_name(),
        ref_klass->external_name(),
        (same_module) ? base_klass->joint_in_module_of_loader(ref_klass) : base_klass->class_in_module_of_loader(),
        (same_module) ? "" : "; ",
        (same_module) ? "" : ref_klass->class_in_module_of_loader());
    } else {
      // Use module specific message returned by verify_class_access_msg().
      Exceptions::fthrow(
        THREAD_AND_LOCATION,
        vmSymbols::java_lang_IllegalAccessError(),
        "%s", msg);
    }
  }
}