void InstanceKlass::initialize_super_interfaces(TRAPS) {
  assert (has_nonstatic_concrete_methods(), "caller should have checked this");
  for (int i = 0; i < local_interfaces()->length(); ++i) {
    InstanceKlass* ik = local_interfaces()->at(i);

    // Initialization is depth first search ie. we start with top of the inheritance tree
    // has_nonstatic_concrete_methods drives searching superinterfaces since it
    // means has_nonstatic_concrete_methods in its superinterface hierarchy
    if (ik->has_nonstatic_concrete_methods()) {
      ik->initialize_super_interfaces(CHECK);
    }

    // Only initialize() interfaces that "declare" concrete methods.
    if (ik->should_be_initialized() && ik->declares_nonstatic_concrete_methods()) {
      ik->initialize(CHECK);
    }
  }
}