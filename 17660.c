jint InstanceKlass::compute_modifier_flags(TRAPS) const {
  jint access = access_flags().as_int();

  // But check if it happens to be member class.
  InnerClassesIterator iter(this);
  for (; !iter.done(); iter.next()) {
    int ioff = iter.inner_class_info_index();
    // Inner class attribute can be zero, skip it.
    // Strange but true:  JVM spec. allows null inner class refs.
    if (ioff == 0) continue;

    // only look at classes that are already loaded
    // since we are looking for the flags for our self.
    Symbol* inner_name = constants()->klass_name_at(ioff);
    if (name() == inner_name) {
      // This is really a member class.
      access = iter.inner_access_flags();
      break;
    }
  }
  // Remember to strip ACC_SUPER bit
  return (access & (~JVM_ACC_SUPER)) & JVM_ACC_WRITTEN_FLAGS;
}