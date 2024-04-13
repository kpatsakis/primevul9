inline DependencyContext InstanceKlass::dependencies() {
  DependencyContext dep_context(&_dep_context);
  return dep_context;
}