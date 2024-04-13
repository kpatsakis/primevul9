void InstanceKlass::remove_dependent_nmethod(nmethod* nm) {
  dependencies().remove_dependent_nmethod(nm);
}