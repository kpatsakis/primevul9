void InstanceKlass::oop_print_on(oop obj, outputStream* st) {
  Klass::oop_print_on(obj, st);

  if (this == vmClasses::String_klass()) {
    typeArrayOop value  = java_lang_String::value(obj);
    juint        length = java_lang_String::length(obj);
    if (value != NULL &&
        value->is_typeArray() &&
        length <= (juint) value->length()) {
      st->print(BULLET"string: ");
      java_lang_String::print(obj, st);
      st->cr();
      if (!WizardMode)  return;  // that is enough
    }
  }

  st->print_cr(BULLET"---- fields (total size %d words):", oop_size(obj));
  FieldPrinter print_field(st, obj);
  do_nonstatic_fields(&print_field);

  if (this == vmClasses::Class_klass()) {
    st->print(BULLET"signature: ");
    java_lang_Class::print_signature(obj, st);
    st->cr();
    Klass* mirrored_klass = java_lang_Class::as_Klass(obj);
    st->print(BULLET"fake entry for mirror: ");
    Metadata::print_value_on_maybe_null(st, mirrored_klass);
    st->cr();
    Klass* array_klass = java_lang_Class::array_klass_acquire(obj);
    st->print(BULLET"fake entry for array: ");
    Metadata::print_value_on_maybe_null(st, array_klass);
    st->cr();
    st->print_cr(BULLET"fake entry for oop_size: %d", java_lang_Class::oop_size(obj));
    st->print_cr(BULLET"fake entry for static_oop_field_count: %d", java_lang_Class::static_oop_field_count(obj));
    Klass* real_klass = java_lang_Class::as_Klass(obj);
    if (real_klass != NULL && real_klass->is_instance_klass()) {
      InstanceKlass::cast(real_klass)->do_local_static_fields(&print_field);
    }
  } else if (this == vmClasses::MethodType_klass()) {
    st->print(BULLET"signature: ");
    java_lang_invoke_MethodType::print_signature(obj, st);
    st->cr();
  }
}