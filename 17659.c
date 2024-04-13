void InstanceKlass::print_on(outputStream* st) const {
  assert(is_klass(), "must be klass");
  Klass::print_on(st);

  st->print(BULLET"instance size:     %d", size_helper());                        st->cr();
  st->print(BULLET"klass size:        %d", size());                               st->cr();
  st->print(BULLET"access:            "); access_flags().print_on(st);            st->cr();
  st->print(BULLET"state:             "); st->print_cr("%s", state_names[_init_state]);
  st->print(BULLET"name:              "); name()->print_value_on(st);             st->cr();
  st->print(BULLET"super:             "); super()->print_value_on_maybe_null(st); st->cr();
  st->print(BULLET"sub:               ");
  Klass* sub = subklass();
  int n;
  for (n = 0; sub != NULL; n++, sub = sub->next_sibling()) {
    if (n < MaxSubklassPrintSize) {
      sub->print_value_on(st);
      st->print("   ");
    }
  }
  if (n >= MaxSubklassPrintSize) st->print("(" INTX_FORMAT " more klasses...)", n - MaxSubklassPrintSize);
  st->cr();

  if (is_interface()) {
    MutexLocker ml(Compile_lock);
    st->print_cr(BULLET"nof implementors:  %d", nof_implementors());
    if (nof_implementors() == 1) {
      st->print_cr(BULLET"implementor:    ");
      st->print("   ");
      implementor()->print_value_on(st);
      st->cr();
    }
  }

  st->print(BULLET"arrays:            "); array_klasses()->print_value_on_maybe_null(st); st->cr();
  st->print(BULLET"methods:           "); methods()->print_value_on(st);                  st->cr();
  if (Verbose || WizardMode) {
    Array<Method*>* method_array = methods();
    for (int i = 0; i < method_array->length(); i++) {
      st->print("%d : ", i); method_array->at(i)->print_value(); st->cr();
    }
  }
  st->print(BULLET"method ordering:   "); method_ordering()->print_value_on(st);      st->cr();
  st->print(BULLET"default_methods:   "); default_methods()->print_value_on(st);      st->cr();
  if (Verbose && default_methods() != NULL) {
    Array<Method*>* method_array = default_methods();
    for (int i = 0; i < method_array->length(); i++) {
      st->print("%d : ", i); method_array->at(i)->print_value(); st->cr();
    }
  }
  if (default_vtable_indices() != NULL) {
    st->print(BULLET"default vtable indices:   "); default_vtable_indices()->print_value_on(st);       st->cr();
  }
  st->print(BULLET"local interfaces:  "); local_interfaces()->print_value_on(st);      st->cr();
  st->print(BULLET"trans. interfaces: "); transitive_interfaces()->print_value_on(st); st->cr();
  st->print(BULLET"constants:         "); constants()->print_value_on(st);         st->cr();
  if (class_loader_data() != NULL) {
    st->print(BULLET"class loader data:  ");
    class_loader_data()->print_value_on(st);
    st->cr();
  }
  st->print(BULLET"host class:        "); host_klass()->print_value_on_maybe_null(st); st->cr();
  if (source_file_name() != NULL) {
    st->print(BULLET"source file:       ");
    source_file_name()->print_value_on(st);
    st->cr();
  }
  if (source_debug_extension() != NULL) {
    st->print(BULLET"source debug extension:       ");
    st->print("%s", source_debug_extension());
    st->cr();
  }
  st->print(BULLET"class annotations:       "); class_annotations()->print_value_on(st); st->cr();
  st->print(BULLET"class type annotations:  "); class_type_annotations()->print_value_on(st); st->cr();
  st->print(BULLET"field annotations:       "); fields_annotations()->print_value_on(st); st->cr();
  st->print(BULLET"field type annotations:  "); fields_type_annotations()->print_value_on(st); st->cr();
  {
    bool have_pv = false;
    // previous versions are linked together through the InstanceKlass
    for (InstanceKlass* pv_node = previous_versions();
         pv_node != NULL;
         pv_node = pv_node->previous_versions()) {
      if (!have_pv)
        st->print(BULLET"previous version:  ");
      have_pv = true;
      pv_node->constants()->print_value_on(st);
    }
    if (have_pv) st->cr();
  }

  if (generic_signature() != NULL) {
    st->print(BULLET"generic signature: ");
    generic_signature()->print_value_on(st);
    st->cr();
  }
  st->print(BULLET"inner classes:     "); inner_classes()->print_value_on(st);     st->cr();
  st->print(BULLET"nest members:     "); nest_members()->print_value_on(st);     st->cr();
  st->print(BULLET"java mirror:       "); java_mirror()->print_value_on(st);       st->cr();
  st->print(BULLET"vtable length      %d  (start addr: " INTPTR_FORMAT ")", vtable_length(), p2i(start_of_vtable())); st->cr();
  if (vtable_length() > 0 && (Verbose || WizardMode))  print_vtable(start_of_vtable(), vtable_length(), st);
  st->print(BULLET"itable length      %d (start addr: " INTPTR_FORMAT ")", itable_length(), p2i(start_of_itable())); st->cr();
  if (itable_length() > 0 && (Verbose || WizardMode))  print_vtable(start_of_itable(), itable_length(), st);
  st->print_cr(BULLET"---- static fields (%d words):", static_field_size());
  FieldPrinter print_static_field(st);
  ((InstanceKlass*)this)->do_local_static_fields(&print_static_field);
  st->print_cr(BULLET"---- non-static fields (%d words):", nonstatic_field_size());
  FieldPrinter print_nonstatic_field(st);
  InstanceKlass* ik = const_cast<InstanceKlass*>(this);
  ik->do_nonstatic_fields(&print_nonstatic_field);

  st->print(BULLET"non-static oop maps: ");
  OopMapBlock* map     = start_of_nonstatic_oop_maps();
  OopMapBlock* end_map = map + nonstatic_oop_map_count();
  while (map < end_map) {
    st->print("%d-%d ", map->offset(), map->offset() + heapOopSize*(map->count() - 1));
    map++;
  }
  st->cr();
}