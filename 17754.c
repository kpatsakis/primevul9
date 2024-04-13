void InstanceKlass::add_dependent_nmethod(nmethod* nm) {
  dependencies().add_dependent_nmethod(nm);
}