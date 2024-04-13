bool InstanceKlass::has_previous_versions_and_reset() {
  bool ret = _has_previous_versions;
  log_trace(redefine, class, iklass, purge)("Class unloading: has_previous_versions = %s",
     ret ? "true" : "false");
  _has_previous_versions = false;
  return ret;
}