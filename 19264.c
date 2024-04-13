Method* LinkResolver::lookup_method_in_klasses(const LinkInfo& link_info,
                                               bool checkpolymorphism,
                                               bool in_imethod_resolve) {
  NoSafepointVerifier nsv;  // Method* returned may not be reclaimed

  Klass* klass = link_info.resolved_klass();
  Symbol* name = link_info.name();
  Symbol* signature = link_info.signature();

  // Ignore overpasses so statics can be found during resolution
  Method* result = klass->uncached_lookup_method(name, signature, Klass::OverpassLookupMode::skip);

  if (klass->is_array_klass()) {
    // Only consider klass and super klass for arrays
    return result;
  }

  InstanceKlass* ik = InstanceKlass::cast(klass);

  // JDK 8, JVMS 5.4.3.4: Interface method resolution should
  // ignore static and non-public methods of java.lang.Object,
  // like clone and finalize.
  if (in_imethod_resolve &&
      result != NULL &&
      ik->is_interface() &&
      (result->is_static() || !result->is_public()) &&
      result->method_holder() == vmClasses::Object_klass()) {
    result = NULL;
  }

  // Before considering default methods, check for an overpass in the
  // current class if a method has not been found.
  if (result == NULL) {
    result = ik->find_method(name, signature);
  }

  if (result == NULL) {
    Array<Method*>* default_methods = ik->default_methods();
    if (default_methods != NULL) {
      result = InstanceKlass::find_method(default_methods, name, signature);
    }
  }

  if (checkpolymorphism && result != NULL) {
    vmIntrinsics::ID iid = result->intrinsic_id();
    if (MethodHandles::is_signature_polymorphic(iid)) {
      // Do not link directly to these.  The VM must produce a synthetic one using lookup_polymorphic_method.
      return NULL;
    }
  }
  return result;
}