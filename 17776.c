void InstanceKlass::remove_dependent_nmethod(nmethod* nm, bool delete_immediately) {
  dependencies().remove_dependent_nmethod(nm, delete_immediately);
}