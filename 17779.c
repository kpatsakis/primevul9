bool InstanceKlass::supers_have_passed_fingerprint_checks() {
  if (java_super() != NULL && !java_super()->has_passed_fingerprint_check()) {
    ResourceMark rm;
    log_trace(class, fingerprint)("%s : super %s not fingerprinted", external_name(), java_super()->external_name());
    return false;
  }

  Array<Klass*>* local_interfaces = this->local_interfaces();
  if (local_interfaces != NULL) {
    int length = local_interfaces->length();
    for (int i = 0; i < length; i++) {
      InstanceKlass* intf = InstanceKlass::cast(local_interfaces->at(i));
      if (!intf->has_passed_fingerprint_check()) {
        ResourceMark rm;
        log_trace(class, fingerprint)("%s : interface %s not fingerprinted", external_name(), intf->external_name());
        return false;
      }
    }
  }

  return true;
}