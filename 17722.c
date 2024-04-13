void InstanceKlass::collect_statistics(KlassSizeStats *sz) const {
  Klass::collect_statistics(sz);

  sz->_inst_size  = wordSize * size_helper();
  sz->_vtab_bytes = wordSize * vtable_length();
  sz->_itab_bytes = wordSize * itable_length();
  sz->_nonstatic_oopmap_bytes = wordSize * nonstatic_oop_map_size();

  int n = 0;
  n += (sz->_methods_array_bytes         = sz->count_array(methods()));
  n += (sz->_method_ordering_bytes       = sz->count_array(method_ordering()));
  n += (sz->_local_interfaces_bytes      = sz->count_array(local_interfaces()));
  n += (sz->_transitive_interfaces_bytes = sz->count_array(transitive_interfaces()));
  n += (sz->_fields_bytes                = sz->count_array(fields()));
  n += (sz->_inner_classes_bytes         = sz->count_array(inner_classes()));
  n += (sz->_nest_members_bytes          = sz->count_array(nest_members()));
  sz->_ro_bytes += n;

  const ConstantPool* cp = constants();
  if (cp) {
    cp->collect_statistics(sz);
  }

  const Annotations* anno = annotations();
  if (anno) {
    anno->collect_statistics(sz);
  }

  const Array<Method*>* methods_array = methods();
  if (methods()) {
    for (int i = 0; i < methods_array->length(); i++) {
      Method* method = methods_array->at(i);
      if (method) {
        sz->_method_count ++;
        method->collect_statistics(sz);
      }
    }
  }
}