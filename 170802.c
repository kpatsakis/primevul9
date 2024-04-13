void InstanceKlass::clean_dependency_context() {
  dependencies().clean_unloading_dependents();
}