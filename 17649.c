bool InstanceKlass::is_same_package_member(const Klass* class2, TRAPS) const {
  if (class2 == this) return true;
  if (!class2->is_instance_klass())  return false;

  // must be in same package before we try anything else
  if (!is_same_class_package(class2))
    return false;

  // As long as there is an outer_this.getEnclosingClass,
  // shift the search outward.
  const InstanceKlass* outer_this = this;
  for (;;) {
    // As we walk along, look for equalities between outer_this and class2.
    // Eventually, the walks will terminate as outer_this stops
    // at the top-level class around the original class.
    bool ignore_inner_is_member;
    const Klass* next = outer_this->compute_enclosing_class(&ignore_inner_is_member,
                                                            CHECK_false);
    if (next == NULL)  break;
    if (next == class2)  return true;
    outer_this = InstanceKlass::cast(next);
  }

  // Now do the same for class2.
  const InstanceKlass* outer2 = InstanceKlass::cast(class2);
  for (;;) {
    bool ignore_inner_is_member;
    Klass* next = outer2->compute_enclosing_class(&ignore_inner_is_member,
                                                    CHECK_false);
    if (next == NULL)  break;
    // Might as well check the new outer against all available values.
    if (next == this)  return true;
    if (next == outer_this)  return true;
    outer2 = InstanceKlass::cast(next);
  }

  // If by this point we have not found an equality between the
  // two classes, we know they are in separate package members.
  return false;
}