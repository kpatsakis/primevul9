bool InstanceKlass::has_nestmate_access_to(InstanceKlass* k, TRAPS) {

  assert(this != k, "this should be handled by higher-level code");

  // Per JVMS 5.4.4 we first resolve and validate the current class, then
  // the target class k.

  InstanceKlass* cur_host = nest_host(CHECK_false);
  if (cur_host == NULL) {
    return false;
  }

  Klass* k_nest_host = k->nest_host(CHECK_false);
  if (k_nest_host == NULL) {
    return false;
  }

  bool access = (cur_host == k_nest_host);

  ResourceMark rm(THREAD);
  log_trace(class, nestmates)("Class %s does %shave nestmate access to %s",
                              this->external_name(),
                              access ? "" : "NOT ",
                              k->external_name());
  return access;
}