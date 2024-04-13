InstanceKlass* InstanceKlass::compute_enclosing_class(bool* inner_is_member, TRAPS) const {
  InstanceKlass* outer_klass = NULL;
  *inner_is_member = false;
  int ooff = 0, noff = 0;
  bool has_inner_classes_attr = find_inner_classes_attr(&ooff, &noff, THREAD);
  if (has_inner_classes_attr) {
    constantPoolHandle i_cp(THREAD, constants());
    if (ooff != 0) {
      Klass* ok = i_cp->klass_at(ooff, CHECK_NULL);
      if (!ok->is_instance_klass()) {
        // If the outer class is not an instance klass then it cannot have
        // declared any inner classes.
        ResourceMark rm(THREAD);
        Exceptions::fthrow(
          THREAD_AND_LOCATION,
          vmSymbols::java_lang_IncompatibleClassChangeError(),
          "%s and %s disagree on InnerClasses attribute",
          ok->external_name(),
          external_name());
        return NULL;
      }
      outer_klass = InstanceKlass::cast(ok);
      *inner_is_member = true;
    }
    if (NULL == outer_klass) {
      // It may be anonymous; try for that.
      int encl_method_class_idx = enclosing_method_class_index();
      if (encl_method_class_idx != 0) {
        Klass* ok = i_cp->klass_at(encl_method_class_idx, CHECK_NULL);
        outer_klass = InstanceKlass::cast(ok);
        *inner_is_member = false;
      }
    }
  }

  // If no inner class attribute found for this class.
  if (NULL == outer_klass) return NULL;

  // Throws an exception if outer klass has not declared k as an inner klass
  // We need evidence that each klass knows about the other, or else
  // the system could allow a spoof of an inner class to gain access rights.
  Reflection::check_for_inner_class(outer_klass, this, *inner_is_member, CHECK_NULL);
  return outer_klass;
}