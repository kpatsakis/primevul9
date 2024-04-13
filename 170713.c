bool InstanceKlass::has_as_permitted_subclass(const InstanceKlass* k) const {
  Thread* current = Thread::current();
  assert(k != NULL, "sanity check");
  assert(_permitted_subclasses != NULL && _permitted_subclasses != Universe::the_empty_short_array(),
         "unexpected empty _permitted_subclasses array");

  if (log_is_enabled(Trace, class, sealed)) {
    ResourceMark rm(current);
    log_trace(class, sealed)("Checking for permitted subclass of %s in %s",
                             k->external_name(), this->external_name());
  }

  // Check that the class and its super are in the same module.
  if (k->module() != this->module()) {
    ResourceMark rm(current);
    log_trace(class, sealed)("Check failed for same module of permitted subclass %s and sealed class %s",
                             k->external_name(), this->external_name());
    return false;
  }

  if (!k->is_public() && !is_same_class_package(k)) {
    ResourceMark rm(current);
    log_trace(class, sealed)("Check failed, subclass %s not public and not in the same package as sealed class %s",
                             k->external_name(), this->external_name());
    return false;
  }

  for (int i = 0; i < _permitted_subclasses->length(); i++) {
    int cp_index = _permitted_subclasses->at(i);
    Symbol* name = _constants->klass_name_at(cp_index);
    if (name == k->name()) {
      log_trace(class, sealed)("- Found it at permitted_subclasses[%d] => cp[%d]", i, cp_index);
      return true;
    }
  }
  log_trace(class, sealed)("- class is NOT a permitted subclass!");
  return false;
}