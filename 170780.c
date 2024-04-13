Method* InstanceKlass::lookup_method_in_all_interfaces(Symbol* name,
                                                       Symbol* signature,
                                                       DefaultsLookupMode defaults_mode) const {
  Array<InstanceKlass*>* all_ifs = transitive_interfaces();
  int num_ifs = all_ifs->length();
  InstanceKlass *ik = NULL;
  for (int i = 0; i < num_ifs; i++) {
    ik = all_ifs->at(i);
    Method* m = ik->lookup_method(name, signature);
    if (m != NULL && m->is_public() && !m->is_static() &&
        ((defaults_mode != DefaultsLookupMode::skip) || !m->is_default_method())) {
      return m;
    }
  }
  return NULL;
}