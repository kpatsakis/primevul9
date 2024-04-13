void InstanceKlass::add_implementor(Klass* k) {
  assert_lock_strong(Compile_lock);
  assert(is_interface(), "not interface");
  // Filter out my subinterfaces.
  // (Note: Interfaces are never on the subklass list.)
  if (InstanceKlass::cast(k)->is_interface()) return;

  // Filter out subclasses whose supers already implement me.
  // (Note: CHA must walk subclasses of direct implementors
  // in order to locate indirect implementors.)
  Klass* sk = k->super();
  if (sk != NULL && InstanceKlass::cast(sk)->implements_interface(this))
    // We only need to check one immediate superclass, since the
    // implements_interface query looks at transitive_interfaces.
    // Any supers of the super have the same (or fewer) transitive_interfaces.
    return;

  Klass* ik = implementor();
  if (ik == NULL) {
    set_implementor(k);
  } else if (ik != this && ik != k) {
    // There is already an implementor. Use itself as an indicator of
    // more than one implementors.
    set_implementor(this);
  }

  // The implementor also implements the transitive_interfaces
  for (int index = 0; index < local_interfaces()->length(); index++) {
    InstanceKlass::cast(local_interfaces()->at(index))->add_implementor(k);
  }
}