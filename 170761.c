bool InstanceKlass::can_be_verified_at_dumptime() const {
  if (major_version() < 50 /*JAVA_6_VERSION*/) {
    return false;
  }
  if (java_super() != NULL && !java_super()->can_be_verified_at_dumptime()) {
    return false;
  }
  Array<InstanceKlass*>* interfaces = local_interfaces();
  int len = interfaces->length();
  for (int i = 0; i < len; i++) {
    if (!interfaces->at(i)->can_be_verified_at_dumptime()) {
      return false;
    }
  }
  return true;
}