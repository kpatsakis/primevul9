void InstanceKlass::add_previous_version(InstanceKlass* scratch_class,
                                         int emcp_method_count) {
  assert(Thread::current()->is_VM_thread(),
         "only VMThread can add previous versions");

  ResourceMark rm;
  log_trace(redefine, class, iklass, add)
    ("adding previous version ref for %s, EMCP_cnt=%d", scratch_class->external_name(), emcp_method_count);

  // Clean out old previous versions for this class
  purge_previous_version_list();

  // Mark newly obsolete methods in remaining previous versions.  An EMCP method from
  // a previous redefinition may be made obsolete by this redefinition.
  Array<Method*>* old_methods = scratch_class->methods();
  mark_newly_obsolete_methods(old_methods, emcp_method_count);

  // If the constant pool for this previous version of the class
  // is not marked as being on the stack, then none of the methods
  // in this previous version of the class are on the stack so
  // we don't need to add this as a previous version.
  ConstantPool* cp_ref = scratch_class->constants();
  if (!cp_ref->on_stack()) {
    log_trace(redefine, class, iklass, add)("scratch class not added; no methods are running");
    // For debugging purposes.
    scratch_class->set_is_scratch_class();
    scratch_class->class_loader_data()->add_to_deallocate_list(scratch_class);
    return;
  }

  if (emcp_method_count != 0) {
    // At least one method is still running, check for EMCP methods
    for (int i = 0; i < old_methods->length(); i++) {
      Method* old_method = old_methods->at(i);
      if (!old_method->is_obsolete() && old_method->on_stack()) {
        // if EMCP method (not obsolete) is on the stack, mark as EMCP so that
        // we can add breakpoints for it.

        // We set the method->on_stack bit during safepoints for class redefinition
        // and use this bit to set the is_running_emcp bit.
        // After the safepoint, the on_stack bit is cleared and the running emcp
        // method may exit.   If so, we would set a breakpoint in a method that
        // is never reached, but this won't be noticeable to the programmer.
        old_method->set_running_emcp(true);
        log_trace(redefine, class, iklass, add)
          ("EMCP method %s is on_stack " INTPTR_FORMAT, old_method->name_and_sig_as_C_string(), p2i(old_method));
      } else if (!old_method->is_obsolete()) {
        log_trace(redefine, class, iklass, add)
          ("EMCP method %s is NOT on_stack " INTPTR_FORMAT, old_method->name_and_sig_as_C_string(), p2i(old_method));
      }
    }
  }

  // Add previous version if any methods are still running.
  // Set has_previous_version flag for processing during class unloading.
  _has_previous_versions = true;
  log_trace(redefine, class, iklass, add) ("scratch class added; one of its methods is on_stack.");
  assert(scratch_class->previous_versions() == NULL, "shouldn't have a previous version");
  scratch_class->link_previous_versions(previous_versions());
  link_previous_versions(scratch_class);
} // end add_previous_version()