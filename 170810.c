inline DependencyContext InstanceKlass::dependencies() {
  DependencyContext dep_context(&_dep_context, &_dep_context_last_cleaned);
  return dep_context;
}