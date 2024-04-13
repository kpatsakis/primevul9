void InstanceKlass::add_implementor(InstanceKlass* ik) {
  if (Universe::is_fully_initialized()) {
    assert_lock_strong(Compile_lock);
  }
  assert(is_interface(), "not interface");
  // Filter out my subinterfaces.
  // (Note: Interfaces are never on the subklass list.)
  if (ik->is_interface()) return;

  // Filter out subclasses whose supers already implement me.
  // (Note: CHA must walk subclasses of direct implementors
  // in order to locate indirect implementors.)
  InstanceKlass* super_ik = ik->java_super();
  if (super_ik != NULL && super_ik->implements_interface(this))
    // We only need to check one immediate superclass, since the
    // implements_interface query looks at transitive_interfaces.
    // Any supers of the super have the same (or fewer) transitive_interfaces.
    return;

  InstanceKlass* iklass = implementor();
  if (iklass == NULL) {
    set_implementor(ik);
  } else if (iklass != this && iklass != ik) {
    // There is already an implementor. Use itself as an indicator of
    // more than one implementors.
    set_implementor(this);
  }

  // The implementor also implements the transitive_interfaces
  for (int index = 0; index < local_interfaces()->length(); index++) {
    local_interfaces()->at(index)->add_implementor(ik);
  }
}