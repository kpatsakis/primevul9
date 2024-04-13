bool InstanceKlass::is_dependent_nmethod(nmethod* nm) {
  return dependencies().is_dependent_nmethod(nm);
}