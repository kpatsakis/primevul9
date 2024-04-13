void InstanceKlass::adjust_default_methods(bool* trace_name_printed) {
  // search the default_methods for uses of either obsolete or EMCP methods
  if (default_methods() != NULL) {
    for (int index = 0; index < default_methods()->length(); index ++) {
      Method* old_method = default_methods()->at(index);
      if (old_method == NULL || !old_method->is_old()) {
        continue; // skip uninteresting entries
      }
      assert(!old_method->is_deleted(), "default methods may not be deleted");
      Method* new_method = old_method->get_new_method();
      default_methods()->at_put(index, new_method);

      if (log_is_enabled(Info, redefine, class, update)) {
        ResourceMark rm;
        if (!(*trace_name_printed)) {
          log_info(redefine, class, update)
            ("adjust: klassname=%s default methods from name=%s",
             external_name(), old_method->method_holder()->external_name());
          *trace_name_printed = true;
        }
        log_debug(redefine, class, update, vtables)
          ("default method update: %s(%s) ",
           new_method->name()->as_C_string(), new_method->signature()->as_C_string());
      }
    }
  }
}