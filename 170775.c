bool InstanceKlass::has_nest_member(JavaThread* current, InstanceKlass* k) const {
  assert(!is_hidden(), "unexpected hidden class");
  if (_nest_members == NULL || _nest_members == Universe::the_empty_short_array()) {
    if (log_is_enabled(Trace, class, nestmates)) {
      ResourceMark rm(current);
      log_trace(class, nestmates)("Checked nest membership of %s in non-nest-host class %s",
                                  k->external_name(), this->external_name());
    }
    return false;
  }

  if (log_is_enabled(Trace, class, nestmates)) {
    ResourceMark rm(current);
    log_trace(class, nestmates)("Checking nest membership of %s in %s",
                                k->external_name(), this->external_name());
  }

  // Check for the named class in _nest_members.
  // We don't resolve, or load, any classes.
  for (int i = 0; i < _nest_members->length(); i++) {
    int cp_index = _nest_members->at(i);
    Symbol* name = _constants->klass_name_at(cp_index);
    if (name == k->name()) {
      log_trace(class, nestmates)("- named class found at nest_members[%d] => cp[%d]", i, cp_index);
      return true;
    }
  }
  log_trace(class, nestmates)("- class is NOT a nest member!");
  return false;
}